#pragma once

#include <stdexcept>
#include <optional>
#include <string>
#include <vector>

class FileGroup;

class Label;

class Report {
public:
    enum class Type {
        ERROR,
        INFO,
        WARNING,
    };

private:
    Report(Type type, std::string &&message, size_t code, std::vector<Label> &&labels,
           std::optional<std::string> &&note);

public:
    friend std::ostream &operator<<(std::ostream &os, const Report &report);

    [[nodiscard]] std::vector<FileGroup> find_file_groups() const;

    [[nodiscard]] const auto &message() const { return _message; };

    [[nodiscard]] const auto &labels() const { return _labels; };

    [[nodiscard]] const auto &note() const { return _note; };

    [[nodiscard]] auto get_type() const { return _type; };

    [[nodiscard]] auto code() const { return _code; };

private:
    [[nodiscard]] std::string type_to_prefix() const;

    [[nodiscard]] std::string type_to_string() const;

private:
    std::optional<std::string> _note;
    std::vector<Label> _labels;
    std::string _message;
    size_t _code;
    Type _type;

public:
    class Builder {
    public:
        Builder &message(std::string &&message);

        Builder &note(std::string &&note);

        Builder &label(Label &&label);

        Builder &code(size_t code);

        Builder &type(Type type);

        Report build();

    private:
        std::optional<std::string> _message{}, _note{};
        std::optional<size_t> _code{};
        std::vector<Label> _labels{};
        std::optional<Type> _type{};
    };
};

class InvalidReportState : public std::runtime_error {
public:
    explicit InvalidReportState(const std::string &field)
            : std::runtime_error("The " + field + " is required to build a report.") {}
};
