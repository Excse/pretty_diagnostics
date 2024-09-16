#pragma once

#include <memory>

class Span {
public:
    Span(size_t start, size_t end, size_t line) : _start(start), _end(end), _line(line) {}

    [[nodiscard]] bool is_inside_span(const Span &span) const;

    [[nodiscard]] Span relative_to(const Span &span) const;

    [[nodiscard]] size_t width() const;

    [[nodiscard]] auto start() const { return _start; };

    [[nodiscard]] auto line() const { return _line; }

    void set_end(size_t index) { _end = index; }

    [[nodiscard]] auto end() const { return _end; }

private:
    size_t _start, _end, _line;
};