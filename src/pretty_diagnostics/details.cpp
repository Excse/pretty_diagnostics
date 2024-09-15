#include "details.h"

#include <regex>

bool AscendingLabels::operator()(const Label *first, const Label *second) const {
    auto difference = (int) first->span().start_index() -
                      (int) second->span().start_index();
    if (difference == 0) {
        return first->span().end_index() < second->span().end_index();
    }

    return difference < 0;
}

bool DescendingLabels::operator()(const Label *first, const Label *second) const {
    auto difference = (int) first->span().start_index() -
                      (int) second->span().start_index();
    if (difference == 0) {
        return first->span().end_index() < second->span().end_index();
    }

    return difference > 0;
}

Details::Details(std::string source, std::string path)
        : _line_spans(), _source(std::move(source)), _path(std::move(path)) {
    Span *current_span = nullptr;
    for (auto index = 0u; index < _source.size(); index++) {
        if (current_span == nullptr) {
            auto new_span = std::make_shared<Span>(this, index, index);
            current_span = new_span.get();
            _line_spans.push_back(new_span);
        }

        const auto &source_char = _source.at(index);
        if (source_char == '\n') {
            current_span->set_end_index(index);
            current_span = nullptr;
            continue;
        }
    }

    if (current_span) {
        current_span->set_end_index(_source.size() - 1);
    }
}

std::string Details::line_source(const Span &span) const {
    auto result = _source.substr(span.start_index(), span.width());
    result = std::regex_replace(result, std::regex("\t"), " ");
    return result;
}

size_t Details::label_line(const Label &label) const {
    for (auto index = 0u; index < _line_spans.size(); index++) {
        const auto &span = _line_spans.at(index);
        if (span->is_inside_span(label.span())) {
            return index;
        }
    }

    throw std::runtime_error("Couldn't find the associated line for this span.");
}