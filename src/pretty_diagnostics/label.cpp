#include "label.h"

Label::Builder &Label::Builder::message(std::string &&message) {
    _message = message;
    return *this;
}

Label::Builder &Label::Builder::file(const std::shared_ptr<File> &file) {
    _file = file;
    return *this;
}

Label::Builder &Label::Builder::span(Span &&span) {
    _span = span;
    return *this;
}

Label Label::Builder::build() {
    if (!_span.has_value()) throw InvalidLabelState("span");
    if (!_file.has_value()) throw InvalidLabelState("file");
    return {std::move(_message), _span.value(), _file.value()};
}
