#pragma once

#include "span.hpp"

namespace pretty_diagnostics {
/**
 * @brief Represents a short textual annotation attached to a `Span` in a source
 *
 * Labels are ordered by their underlying `Span` to support stable grouping
 * and rendering of diagnostics
 */
class Label {
public:
    /**
     * @brief Constructs a label with a human-readable message and the span it refers to
     *
     * @param text Short message to display next to the span in the rendered output
     * @param span Source span this label highlights
     */
    Label(std::string text, Span span);

    /**
     * @brief Orders labels by their span to allow placement within a line/group
     *
     * @param lhs Left-hand label
     * @param rhs Right-hand label
     *
     * @return True if @p lhs's span starts before @p rhs's span
     */
    friend bool operator<(const Label& lhs, const Label& rhs) { return lhs._span < rhs._span; }

    /**
     * @brief Less-than-or-equal comparison derived from `>=`
     *
     * @param lhs Left-hand label
     * @param rhs Right-hand label
     *
     * @return True if @p lhs is not greater than @p rhs
     */
    friend bool operator<=(const Label& lhs, const Label& rhs) { return rhs >= lhs; }

    /**
     * @brief Greater-than comparison derived from `<`
     *
     * @param lhs Left-hand label
     * @param rhs Right-hand label
     *
     * @return True if @p lhs is ordered after @p rhs
     */
    friend bool operator>(const Label& lhs, const Label& rhs) { return rhs < lhs; }

    /**
     * @brief Greater-than-or-equal comparison derived from `<`
     *
     * @param lhs Left-hand label
     * @param rhs Right-hand label
     *
     * @return True if @p lhs is not less than @p rhs
     */
    friend bool operator>=(const Label& lhs, const Label& rhs) { return !(lhs < rhs); }

    /**
     * @brief Returns the label text
     *
     * @return Human-readable message associated with this label
     */
    [[nodiscard]] const std::string& text() const { return _text; }

    /**
     * @brief Returns the span associated with this label
     *
     * @return Span this label highlights
     */
    [[nodiscard]] const Span& span() const { return _span; }

private:
    std::string _text;
    Span _span;
};
} // namespace pretty_diagnostics

// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
