#include "report.h"

#include <iostream>
#include <iomanip>
#include <map>

#include "file_group.h"
#include "label.h"
#include "utils.h"

Report::Report(Type type, std::string &&message, size_t code, std::vector<Label> &&labels,
               std::optional<std::string> &&note)
        : _note(std::move(note)), _labels(std::move(labels)), _message(std::move(message)),
          _code(code), _type(type) {}

std::ostream &operator<<(std::ostream &os, const Report &report) {
    os << "[" << report.type_to_prefix() << std::setw(4) << std::setfill('0') << report.code() << "] ";
    os << report.type_to_string() << ":" << " " << report.message() << "\n";

    auto file_groups = report.find_file_groups();

    auto biggest_number = 0u;
    for (const auto &file_group: file_groups) {
        auto file_biggest_number = file_group.get_biggest_displayed_number();
        if (file_biggest_number > biggest_number) {
            biggest_number = file_biggest_number;
        }
    }

    auto biggest_number_width = std::to_string(biggest_number).length();
    auto spaces_prefix = std::string(biggest_number_width + 3, ' ');

    os << spaces_prefix << "╭";
    for (auto index = 0u; index < file_groups.size(); index++) {
        const auto &file_group = file_groups.at(index);
        file_group.print(os, spaces_prefix);

        if (index != file_groups.size() - 1) {
            os << spaces_prefix << "·";
            os << "\n";

            os << spaces_prefix << "├";
        }
    }

    os << spaces_prefix << "·";
    os << "\n";

    if (report.note().has_value()) {
        os << spaces_prefix << "│  Note: " << report.note().value() << "\n";
    }

    auto dashes_prefix = biggest_number_width + 3;
    os << repeat_string("─", dashes_prefix) << "╯";
    os << "\n";

    return os;
}

auto Report::find_file_groups() const -> std::vector<FileGroup> {
    std::map<std::shared_ptr<File>, Labels> group_mappings;
    for (const auto &label : _labels) {
        auto result = group_mappings.find(label.file());
        if (result == group_mappings.end()) {
            group_mappings[label.file()] = {&label};
        } else {
            result->second.push_back(&label);
        }
    }

    std::vector<FileGroup> file_groups(group_mappings.size());
    for (const auto &[details, labels]: group_mappings) {
        file_groups.emplace_back(details, labels);
    }

    return file_groups;
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

Report::Builder &Report::Builder::label(Label &&label) {
    _labels.push_back(label);
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
    return {_type.value(), std::move(_message.value()), _code.value(), std::move(_labels), std::move(_note)};
}