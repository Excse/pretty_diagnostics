#pragma once

#include <memory>
#include <vector>

#include "span.h"

class File;

class Label;

using Labels = std::vector<const Label *>;

class LabelGroup {
public:
    LabelGroup(std::shared_ptr<File> details, Labels labels);

    void print(std::ostream &output, const std::string &spaces_prefix) const;

    void print_colored_source_line(std::ostream &output, const Span &label_span,
                                   const Labels &labels) const;

    [[nodiscard]] Labels find_labels_in_line(size_t line_index) const;

    [[nodiscard]] const auto &first_label() const { return *_first_label; };

    [[nodiscard]] const auto &last_label() const { return *_last_label; };

    [[nodiscard]] const auto &details() const { return *_details; };

    [[nodiscard]] const auto &labels() const { return _labels; };

    [[nodiscard]] static std::vector<Labels> find_label_levels(const Labels &labels);

    [[nodiscard]] static Labels find_remove_overlapping_labels(Labels &labels);

    static void print_labels_level(const std::vector<Labels> &level_labels,
                                   size_t current_level,
                                   const Span &line_span,
                                   std::ostream &output,
                                   const std::string &spaces_prefix);

private:
    std::shared_ptr<File> _details;
    const Label *_first_label;
    const Label *_last_label;
    Labels _labels;
};
