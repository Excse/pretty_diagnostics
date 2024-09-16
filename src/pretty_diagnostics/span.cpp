#include "span.h"

Span Span::relative_to(const Span &span) const {
    return {_start - span._start, _end - span._start};
}

bool Span::is_inside_span(const Span &span) const {
    return (_start <= span._start) && (_end >= span._end);
}

size_t Span::width() const {
    return _end - _start;
}
