#include "pretty_diagnostics/label_group.h"

#include <iostream>
#include <limits>

constexpr int64_t MAX_COL = std::numeric_limits<int64_t>::max();
constexpr int64_t MIN_COL = 0;

constexpr int64_t MAX_LINE_DIFFERENCE = 1;

bool LabelGroup::can_fit(const Label &label) const {
    if (_labels.empty()) return true;

    Span region{{min_row() - MAX_LINE_DIFFERENCE, MIN_COL},
                {max_row() + MAX_LINE_DIFFERENCE, MAX_COL}, true};
    return region.contains(label.span());
}

void LabelGroup::insert(Label &&label) {
    const auto sort_label = [](const Label &first, const Label &second) {
        if (first.span().start().row < second.span().end().row) {
            // TODO: As there is no multiline support, this will result in first being sorted before second.
            // FIXME: MULTINE
            return true;
        }

        // TODO: As there is no multiline support, the span that begins the first will be sorted first.
        // FIXME: MULTINE
        return first.span().start().column < second.span().start().column;
    };

    _labels.push_back(label);

    // Always sort the vector to gurantee that min_row and max_row works. Also, the final printing relies on that fact.
    std::sort(_labels.begin(), _labels.end(), sort_label);
}

int64_t LabelGroup::min_row() const {
    // TODO: Doesn't work for multiline later
    // FIXME: MULTINE
    return _labels.begin()->span().start().row;
}

int64_t LabelGroup::max_row() const {
    // TODO: Doesn't work for multiline later
    // FIXME: MULTINE
    return _labels.end()->span().end().row;
}
