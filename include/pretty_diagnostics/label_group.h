#pragma once

#include <memory>
#include <vector>

#include "pretty_diagnostics/label.h"
#include "pretty_diagnostics/span.h"

class File;

class LabelGroup {
public:
    [[nodiscard]] bool can_fit(const Label &label) const;

    void insert(Label &&label);

    [[nodiscard]] int64_t max_row() const;

    [[nodiscard]] int64_t min_row() const;

    [[nodiscard]] const auto &labels() { return _labels; };

private:
    std::vector<Label> _labels{};
};
