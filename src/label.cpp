#include "pretty_diagnostics/label.h"

Label::Builder &Label::Builder::message(std::string &&message) {
    _message = message;
    return *this;
}

Label::Builder &Label::Builder::span(Span &&span) {
    // FIXME: Currently no multiline spans are allowed.
    if (span.ignore_multiline()) throw std::invalid_argument("No multiline spans are allowed.");

    _span = span;
    return *this;
}

Label Label::Builder::build() {
    if (!_span.has_value()) throw InvalidLabelState("span");
    return {std::move(_message), _span.value()};
}
