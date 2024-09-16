#include "pretty_diagnostics/file.h"

#include <algorithm>

bool AscendingLabels::operator()(const Label *first, const Label *second) const {
    auto difference = (int) first->span().start() - (int) second->span().start();
    if (difference == 0) {
        return first->span().end() < second->span().end();
    }

    return difference < 0;
}

bool DescendingLabels::operator()(const Label *first, const Label *second) const {
    auto difference = (int) first->span().start() - (int) second->span().start();
    if (difference == 0) {
        return first->span().end() < second->span().end();
    }

    return difference > 0;
}

File::File(std::string source, std::string path)
        : _line_spans(), _source(std::move(source)), _path(std::move(path)) {
    size_t line = 0, start = 0;
    for (auto index = 0u; index < _source.size(); index++) {
        if (_source[index] != '\n' && (index != _source.size() - 1)) {
            continue;
        }

        auto span = std::make_shared<Span>(start, index, line);
        _line_spans.push_back(span);

        start = index + 1;
        line++;
    }
}

std::string File::line_source(const Span &span) const {
    auto result = _source.substr(span.start(), span.width());
    std::replace(result.begin(), result.end(), '\t', ' ');
    return result;
}