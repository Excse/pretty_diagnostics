#ifndef PRETTY_DIAGNOSTICS_LABEL_H
#define PRETTY_DIAGNOSTICS_LABEL_H

#include <optional>
#include <string>

#include "span.h"

class Label {
private:
    Label(std::optional<std::string> &&message, Span &&span);

public:
    [[nodiscard]] const auto &message() const { return _message; };

    [[nodiscard]] const auto &span() const { return _span; };

    [[nodiscard]] auto line() const { return _line; };

private:
    std::optional<std::string> _message;
    size_t _line;
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
            : std::runtime_error("The " + field + " is required to build a label.") {}
};

#endif //PRETTY_DIAGNOSTICS_LABEL_H
