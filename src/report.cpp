#include "pretty_diagnostics/report.h"

#include <iostream>
#include <iomanip>
#include <map>

std::ostream &operator<<(std::ostream &os, const Report &report) {
    os << "[" << report.type_to_prefix() << std::setw(4) << std::setfill('0') << report.code() << "] ";
    os << report.type_to_string() << ":" << " " << report.message() << "\n";
    return os;
}

std::string Report::type_to_prefix() const {
    switch (_type) {
        case Type::ERROR: return "E";
        case Type::INFO: return "I";
        case Type::WARNING: return "W";
        default: throw std::invalid_argument("This report type is not implemented yet.");
    }
}

std::string Report::type_to_string() const {
    switch (_type) {
        case Type::ERROR: return "Error";
        case Type::INFO: return "Info";
        case Type::WARNING: return "Warning";
        default: throw std::invalid_argument("This report type is not implemented yet.");
    }
}

Report::Builder &Report::Builder::message(std::string &&message) {
    _message = message;
    return *this;
}

Report::Builder &Report::Builder::note(std::string &&note) {
    _note = note;
    return *this;
}

Report::Builder &Report::Builder::group(FileGroup &&group) {
    _groups.push_back(group);
    return *this;
}

Report::Builder &Report::Builder::type(Type type) {
    _type = type;
    return *this;
}

Report::Builder &Report::Builder::code(size_t code) {
    _code = code;
    return *this;
}

Report Report::Builder::build() {
    if (!_message.has_value()) throw InvalidReportState("message");
    if (!_type.has_value()) throw InvalidReportState("type");
    if (!_code.has_value()) throw InvalidReportState("code");
    return {_type.value(), std::move(_message.value()), _code.value(), std::move(_groups), std::move(_note)};
}
