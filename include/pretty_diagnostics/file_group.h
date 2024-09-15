#ifndef PRETTY_ERRORS_LIBRARY_H
#define PRETTY_ERRORS_LIBRARY_H

#include "label_group.h"

class Details;

using Labels = std::vector<const Label *>;

class FileGroup {
public:
    FileGroup(const std::shared_ptr<Details> &details, Labels labels);

    FileGroup() : _label_groups(), _details() {}

    void print(std::ostream &output, const std::string &spaces_prefix) const;

    [[nodiscard]] size_t get_biggest_displayed_number() const;

    [[nodiscard]] const auto &label_groups() const { return _label_groups; };

    [[nodiscard]] const auto &details() const { return *_details; };

private:
    std::vector<LabelGroup> _label_groups;
    std::shared_ptr<Details> _details;
};

#endif //PRETTY_ERRORS_LIBRARY_H
