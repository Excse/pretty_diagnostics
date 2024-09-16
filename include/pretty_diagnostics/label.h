#pragma once

#include <optional>
#include <string>
#include <utility>

#include "span.h"

class File;

class Label {
private:
    Label(std::optional<std::string> &&message, Span span)
        : _message(std::move(message)), _span(span) {}

public:
    [[nodiscard]] const auto &message() const { return _message; };

    [[nodiscard]] const auto &span() const { return _span; };

private:
    std::optional<std::string> _message;
    Span _span;

public:
    class Builder {
    public:
        Builder &message(std::string &&message);

        Builder &span(Span &&span);

        Label build();

    private:
        std::optional<std::string> _message{};
        std::optional<Span> _span{};
    };
};

class InvalidLabelState : public std::runtime_error {
public:
    explicit InvalidLabelState(const std::string &field)
        : std::runtime_error("The field \"" + field + "\" is required to build a label.") {}
};
