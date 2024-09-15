#include "file_group.h"

#include "details.h"

constexpr int32_t DISPLAYED_LINE_PADDING = 1;

FileGroup::FileGroup(const std::shared_ptr<Details> &details, Labels labels)
        : _details(details) {
    if (labels.empty()) {
        throw std::invalid_argument("Cannot find label current_labels if there are no labels_collection.");
    }

    std::vector<Labels> labels_collection;
    auto current_labels = &labels_collection.emplace_back();

    auto ascending_labels(labels);
    std::sort(ascending_labels.begin(), ascending_labels.end(), AscendingLabels());

    auto last_line = labels.front()->line();
    for (const auto &label: ascending_labels) {
        auto label_line = label->line();
        auto line_difference = (int32_t) label_line - (int32_t) last_line;
        if (line_difference > DISPLAYED_LINE_PADDING) {
            current_labels = &labels_collection.emplace_back();
        }

        current_labels->push_back(label);
        last_line = label_line;
    }

    for (const auto &collected_labels: labels_collection) {
        _label_groups.emplace_back(details, collected_labels);
    }
}

void FileGroup::print(std::ostream &output, const std::string &spaces_prefix) const {
    output << "─[" << _details->path() << "]";
    output << "\n";

    output << spaces_prefix << "·";
    output << "\n";

    for (auto index = 0u; index < _label_groups.size(); index++) {
        const auto &labels_group = _label_groups.at(index);
        labels_group.print(output, spaces_prefix);

        if (index != _label_groups.size() - 1) {
            output << spaces_prefix << "⋮";
            output << "\n";
        }
    }
}

auto FileGroup::get_biggest_displayed_number() const -> size_t {
    auto biggest_number = 0u;
    for (const auto &labels_group: _label_groups) {
        auto &last_label = labels_group.last_label();
        auto line_number = last_label.line();
        if (biggest_number < line_number) {
            biggest_number = line_number;
        }
    }

    // Add 1 to get a number starting at 1. Add the line padding to get the last line displayed by
    // the report.
    biggest_number += 1 + DISPLAYED_LINE_PADDING;

    return biggest_number;
}
