#include "span.h"

Span Span::relative_to(const Span &span) const {
    return {span.details(), _start_index - span._start_index,
            _end_index - span._start_index};
}

bool Span::is_inside_span(const Span &span) const {
    return (_start_index <= span._start_index) && (_end_index >= span._end_index);
}

size_t Span::width() const {
    return _end_index - _start_index;
}
