#pragma once

#include <optional>
#include <string>
#include <utility>

#include "span.h"

class File;

class Label {
private:
    Label(std::optional<std::string> &&message, Span span, std::shared_ptr<File> file)
            : _message(std::move(message)), _file(std::move(file)), _span(span) {}

public:
    [[nodiscard]] const auto &message() const { return _message; };

    [[nodiscard]] const auto &span() const { return _span; };

    [[nodiscard]] const auto &file() const { return _file; };

private:
    std::optional<std::string> _message;
    std::shared_ptr<File> _file;
    Span _span;

public:
    class Builder {
    public:
        Builder &file(const std::shared_ptr<File> &span);

        Builder &message(std::string &&message);

        Builder &span(Span &&span);

        Label build();

    private:
        std::optional<std::shared_ptr<File>> _file{};
        std::optional<std::string> _message{};
        std::optional<Span> _span{};
    };
};

class InvalidLabelState : public std::runtime_error {
public:
    explicit InvalidLabelState(const std::string &field)
            : std::runtime_error("The " + field + " is required to build a label.") {}
};
