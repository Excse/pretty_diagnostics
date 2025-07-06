#pragma once

#include <stdexcept>
#include <optional>
#include <string>
#include <vector>

#include "file_group.h"

class Report {
public:
    enum class Type {
        ERROR,
        INFO,
        WARNING,
    };

private:
    Report(Type type, std::string &&message, size_t code, std::vector<FileGroup> &&groups,
           std::optional<std::string> &&note) :
            _note(std::move(note)), _groups(std::move(groups)), _message(std::move(message)),
            _code(code), _type(type) {}

public:
    friend std::ostream &operator<<(std::ostream &os, const Report &report);

    [[nodiscard]] const auto &message() const { return _message; };

    [[nodiscard]] const auto &groups() const { return _groups; };

    [[nodiscard]] const auto &note() const { return _note; };

    [[nodiscard]] auto type() const { return _type; };

    [[nodiscard]] auto code() const { return _code; };

private:
    [[nodiscard]] std::string type_to_prefix() const;

    [[nodiscard]] std::string type_to_string() const;

private:
    std::optional<std::string> _note;
    std::vector<FileGroup> _groups;
    std::string _message;
    size_t _code;
    Type _type;

public:
    class Builder {
    public:
        Builder &message(std::string &&message);

        Builder &group(FileGroup &&group);

        Builder &note(std::string &&note);

        Builder &code(size_t code);

        Builder &type(Type type);

        Report build();

    private:
        std::optional<std::string> _message{}, _note{};
        std::vector<FileGroup> _groups{};
        std::optional<size_t> _code{};
        std::optional<Type> _type{};
    };
};

class InvalidReportState final : public std::runtime_error {
public:
    explicit InvalidReportState(const std::string &field)
            : std::runtime_error("The field \"" + field + "\" is required to build a report.") {}
};
