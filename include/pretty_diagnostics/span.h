#pragma once

#include <memory>

struct Location {
    int64_t row, column;
};

class Span {
public:
    Span(Location start, Location end) : Span(start, end, false) {}

    Span(Location start, Location end, bool ignore_multine);

    [[nodiscard]] bool contains(const Span &inner) const;

    [[nodiscard]] size_t column_width() const;

    [[nodiscard]] const auto &ignore_multiline() const { return _ignore_multiline; }

    [[nodiscard]] const auto &start() const { return _start; };

    [[nodiscard]] const auto &end() const { return _end; }

private:
    bool _ignore_multiline;
    Location _start, _end;
};