#ifndef PRETTY_DIAGNOSTICS_SPAN_H
#define PRETTY_DIAGNOSTICS_SPAN_H

#include <memory>

class Details;

class Span {
public:
    Span(std::shared_ptr<Details> details, size_t start_index, size_t end_index)
            : _details(std::move(details)), _start_index(start_index), _end_index(end_index) {}

    [[nodiscard]] bool is_inside_span(const Span &span) const;

    [[nodiscard]] Span relative_to(const Span &span) const;

    [[nodiscard]] size_t width() const;

    [[nodiscard]] auto start_index() const { return _start_index; };

    void set_end_index(size_t index) { _end_index = index; }

    [[nodiscard]] auto end_index() const { return _end_index; }

    [[nodiscard]] auto details() const { return _details; };

private:
    std::shared_ptr<Details> _details;
    size_t _start_index, _end_index;
};

#endif //PRETTY_DIAGNOSTICS_SPAN_H
