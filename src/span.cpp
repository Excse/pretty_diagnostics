#include "pretty_diagnostics/span.h"

Span::Span(Location start, Location end, bool ignore_multiline)
    : _start(start), _end(end), _ignore_multiline(ignore_multiline) {
    if (_start.row > _end.row) {
        throw std::invalid_argument("The start row is before the end row.");
    }

    // TODO: Maybe add support for multiline
    auto multiline = _start.row != _end.row;
    if (multiline && !ignore_multiline) throw std::invalid_argument("Multiline is not supported.");

    if (_start.column >= _end.column) {
        throw std::invalid_argument("Can't create a empty or negative Span.");
    }
}

bool Span::contains(const Span &inner) const {
    return _start.column <= inner._start.column &&
           _start.row <= inner._start.row &&
           _end.column >= inner._end.column &&
           _end.row >= inner._end.row;
}

size_t Span::column_width() const {
    return _end.column - _start.column;
}