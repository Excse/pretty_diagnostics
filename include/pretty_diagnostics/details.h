#ifndef PRETTY_DIAGNOSTICS_DETAILS_H
#define PRETTY_DIAGNOSTICS_DETAILS_H

#include <memory>
#include <vector>
#include <string>

#include "label.h"
#include "span.h"

struct AscendingLabels {
    bool operator()(const Label *first, const Label *second) const;
};

struct DescendingLabels {
    bool operator()(const Label *first, const Label *second) const;
};

class Details {
public:
    Details(std::string source, std::string path);

    [[nodiscard]] std::string line_source(const Span &span) const;

    [[nodiscard]] size_t label_line(const Label &label) const;

    [[nodiscard]] const auto &line_spans() const { return _line_spans; };

    [[nodiscard]] const auto &source() const { return _source; };

    [[nodiscard]] const auto &path() const { return _path; };

private:
    std::vector<std::shared_ptr<Span>> _line_spans;
    std::string _source;
    std::string _path;
};

#endif //PRETTY_DIAGNOSTICS_DETAILS_H
