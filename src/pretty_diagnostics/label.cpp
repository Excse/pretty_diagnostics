#include "label.h"

#include "file_group.h"

Label::Label(std::optional<std::string> &&message, Span &&span)
        : _message(std::move(message)), _span(std::move(span)) {
    _line = _span.details()->label_line(*this);
}

Label::Builder &Label::Builder::message(std::string &&message) {
    _message = message;
    return *this;
}

Label::Builder &Label::Builder::span(Span &&span) {
    _span = span;
    return *this;
}

Label Label::Builder::build() {
    if (!_span.has_value()) throw InvalidLabelState("A span is required to build a label.");
    return {std::move(_message), std::move(_span.value())};
}