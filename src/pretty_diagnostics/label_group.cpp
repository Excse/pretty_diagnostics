#include "label_group.h"

#include <iostream>
#include <iomanip>
#include <map>

#include "details.h"
#include "label.h"

constexpr size_t DISPLAYED_LINE_PADDING = 1;

LabelGroup::LabelGroup(std::shared_ptr<Details> details, std::vector<const Label *> labels)
        : _details(std::move(details)), _first_label(), _last_label(),
          _labels(std::move(labels)) {
    if (_labels.empty()) {
        throw std::invalid_argument("Couldn't find the last labels as there are no labels.");
    }

    auto ascending_labels(_labels);
    std::sort(ascending_labels.begin(), ascending_labels.end(), AscendingLabels());

    _first_label = ascending_labels.front();
    _last_label = ascending_labels.back();
}

void LabelGroup::print(std::ostream &output, const std::string &spaces_prefix) const {
    auto first_line = _first_label->line();
    auto last_line = _last_label->line();

    auto beginning_line = 0u;
    if (first_line >= DISPLAYED_LINE_PADDING) {
        beginning_line = first_line - DISPLAYED_LINE_PADDING;
    }

    auto ending_line = last_line + DISPLAYED_LINE_PADDING;
    if (ending_line >= _details->line_spans().size()) {
        ending_line = last_line;
    }

    for (auto line_index = beginning_line; line_index <= ending_line; line_index++) {
        const auto &line_span = _details->line_spans()[line_index];

        auto line_number = line_index + 1;
        output << "  " << std::setw(spaces_prefix.length() - 3) << std::setfill(' ') << line_number << " │  ";

        auto labels = find_labels_in_line(line_index);
        print_colored_source_line(output, *line_span, labels);

        if (labels.empty()) {
            continue;
        }

        auto label_levels = find_label_levels(labels);
        for (auto index = 0u; index < label_levels.size(); index++) {
            print_labels_level(label_levels, index, *line_span, output, spaces_prefix);
        }
    }
}

auto LabelGroup::find_label_levels(const Labels &labels) -> std::vector<Labels> {
    auto descending_labels(labels);
    std::sort(descending_labels.begin(), descending_labels.end(), DescendingLabels());

    std::vector<Labels> level_labels;

    auto current_labels = descending_labels;
    while (true) {
        auto overlapping_labels = find_remove_overlapping_labels(current_labels);
        level_labels.push_back(current_labels);

        if (overlapping_labels.empty()) {
            break;
        }

        current_labels = overlapping_labels;
    }

    return level_labels;
}

auto LabelGroup::find_remove_overlapping_labels(Labels &labels) -> Labels {
    if (labels.empty()) {
        return {};
    }

    Labels overlapping_labels;

    auto current_label = labels.front();
    for (auto iterator = labels.begin() + 1; iterator < labels.end(); iterator++) {
        auto next_label = *iterator;

        if (next_label->span().end_index() < current_label->span().start_index()) {
            current_label = next_label;
        } else {
            overlapping_labels.push_back(next_label);
            iterator = labels.erase(iterator) - 1;
        }
    }

    return overlapping_labels;
}

void LabelGroup::print_labels_level(const std::vector<Labels> &level_labels,
                                    size_t current_level,
                                    const Span &line_span,
                                    std::ostream &output,
                                    const std::string &spaces_prefix) {
    std::map<size_t, const Label *> next_label_startings, next_label_endings;
    for (auto index = current_level + 1; index < level_labels.size(); index++) {
        auto &labels = level_labels.at(index);
        for (const auto &label: labels) {
            const auto relative_span = label->span().relative_to(line_span);
            next_label_startings[relative_span.start_index()] = label;
            next_label_endings[relative_span.end_index()] = label;
        }
    }

    auto &current_labels = level_labels.at(current_level);

    std::map<size_t, const Label *> current_label_startings;
    for (const auto &label: current_labels) {
        const auto relative_span = label->span().relative_to(line_span);
        current_label_startings[relative_span.start_index()] = label;
    }

    output << spaces_prefix << "·  ";

    size_t last_end_index = 0u;
    for (auto index = 0u; index < line_span.width(); index++) {
        if (next_label_endings.contains(index) || next_label_startings.contains(index)) {
            std::cout << "│";
            continue;
        }

        if (!current_label_startings.contains(index)) {
            if (index == last_end_index && index != 0) {
                std::cout << "╯";
            } else if (index < last_end_index) {
                std::cout << "─";
            } else {
                output << " ";
            }

            continue;
        }

        auto label = current_label_startings.at(index);
        auto relative_span = label->span().relative_to(line_span);

        if (last_end_index >= index && index != 0) {
            if (label->message()) {
                std::cout << "┤";
            } else {
                std::cout << "╯";
            }
        } else if (relative_span.end_index() > index) {
            if (label->message()) {
                std::cout << "├";
            } else {
                std::cout << "╰";
            }
        } else if (label->message()) {
            std::cout << "│";
        } else {
            std::cout << "^";
        }

        last_end_index = relative_span.end_index();
    }

    output << "\n";

    for (const auto &label: current_labels) {
        if (!label->message()) {
            continue;
        }

        output << spaces_prefix << "·  ";

        const auto relative_span = label->span().relative_to(line_span);
        for (auto index = 0u; index < relative_span.start_index(); index++) {
            if (next_label_endings.contains(index) || next_label_startings.contains(index) ||
                current_label_startings.contains(index)) {
                std::cout << "│";
                continue;
            }

            output << " ";
        }

        std::cout << "╰─▶ ";
        std::cout << label->message().value();
        output << "\n";
    }
}

void LabelGroup::print_colored_source_line(std::ostream &output, const Span &label_span,
                                           const Labels &labels) const {
    const auto source = _details->line_source(label_span);

    std::map<size_t, const Label *> mapped_labels;
    for (const auto &label: labels) {
        const auto &line_span = _details->line_spans()[label->line()];
        auto relative_span = label->span().relative_to(*line_span);
        mapped_labels[relative_span.start_index()] = label;
    }

    for (auto char_index = 0u; char_index < source.length(); char_index++) {
        const auto &current_char = source.at(char_index);
        if (!mapped_labels.contains(char_index)) {
            output << current_char;
            continue;
        }

        const auto &label = mapped_labels.at(char_index);
        output << current_char;

        auto starting_index = char_index;
        for (char_index++; char_index < starting_index + label->span().width() + 1;
             char_index++) {
            if (mapped_labels.contains(char_index)) {
                break;
            }

            const auto &label_char = source.at(char_index);
            output << label_char;
        }
        char_index--;
    }

    output << "\n";
}

auto LabelGroup::find_labels_in_line(size_t line_index) const -> Labels {
    Labels result;

    const auto &line_span = _details->line_spans().at(line_index);
    for (const auto &label: _labels) {
        if (line_span->is_inside_span(label->span())) {
            result.push_back(label);
        }
    }

    return result;
}