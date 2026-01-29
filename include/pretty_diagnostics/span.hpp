#pragma once

#include "source.hpp"

namespace pretty_diagnostics {
/**
 * @brief Represents a contiguous region within a `Source`
 *
 * A span is defined by start and end `Location`s and can be constructed from
 * coordinates or linear indices. Spans are comparable by their start location
 */
class Span {
public:
    /**
     * @brief Constructs a span from explicit start and end locations within the same source
     *
     * @param source Backing source shared pointer
     * @param start Start location (inclusive)
     * @param end End location (exclusive)
     */
    Span(const std::shared_ptr<Source>& source, const Location& start, const Location& end);

    /**
     * @brief Constructs a span from row/column coordinates
     *
     * @param source Backing source shared pointer
     * @param start_row 0-based start line number
     * @param start_column 0-based start column number
     * @param end_row 0-based end line number
     * @param end_column 0-based end column number
     */
    Span(const std::shared_ptr<Source>& source, size_t start_row, size_t start_column, size_t end_row, size_t end_column);

    /**
     * @brief Constructs a span from 0-based character indices into the source content
     *
     * @param source Backing source shared pointer
     * @param start_index 0-based start index (inclusive)
     * @param end_index 0-based end index (exclusive)
     */
    Span(const std::shared_ptr<Source>& source, size_t start_index, size_t end_index);

    /**
     * @brief Orders spans by their start location
     *
     * @param lhs Left-hand span
     * @param rhs Right-hand span
     *
     * @return True if @p lhs starts before @p rhs
     */
    friend bool operator<(const Span& lhs, const Span& rhs) { return lhs._start < rhs._start; }

    /**
     * @brief Less-than-or-equal comparison derived from `>=`
     *
     * @param lhs Left-hand span
     * @param rhs Right-hand span
     *
     * @return True if @p lhs is not greater than @p rhs
     */
    friend bool operator<=(const Span& lhs, const Span& rhs) { return rhs >= lhs; }

    /**
     * @brief Greater-than comparison derived from `<`
     *
     * @param lhs Left-hand span
     * @param rhs Right-hand span
     *
     * @return True if @p lhs starts after @p rhs
     */
    friend bool operator>(const Span& lhs, const Span& rhs) { return rhs < lhs; }

    /**
     * @brief Greater-than-or-equal comparison derived from `<`
     *
     * @param lhs Left-hand span
     * @param rhs Right-hand span
     *
     * @return True if @p lhs is not less than @p rhs
     */
    friend bool operator>=(const Span& lhs, const Span& rhs) { return !(lhs < rhs); }

    /**
     * @brief Equality compares source, start and end
     *
     * @param lhs Left-hand span
     * @param rhs Right-hand span
     *
     * @return True if both spans refer to the same source and coordinates
     */
    friend bool operator==(const Span& lhs, const Span& rhs) {
        return lhs._source == rhs._source && lhs._start == rhs._start && lhs._end == rhs._end;
    }

    /**
     * @brief Inequality based on `==`
     *
     * @param lhs Left-hand span
     * @param rhs Right-hand span
     *
     * @return True if the spans differ
     */
    friend bool operator!=(const Span& lhs, const Span& rhs) { return !(lhs == rhs); }

    /**
     * @brief Combines this span with another span into a single continuous span
     *
     * The resulting span starts at the earliest start location of the two
     * spans and ends at the latest end location of the two spans.
     *
     * @param other The span to combine with
     *
     * @return A new span that encompasses both spans
     */
    [[nodiscard]] Span join(const Span& other) const;

    /**
     * @brief Returns true if this span intersects the other span
     *
     * @param other Other span to test against
     *
     * @return True if the spans overlap; otherwise false
     */
    [[nodiscard]] bool intersects(const Span& other) const;

    /**
     * @brief Extracts the substring of the source covered by this span
     *
     * @return The substring of the backing source covered by this span
     */
    [[nodiscard]] std::string substr() const;

    /**
     * @brief Returns the number of characters covered by this span
     *
     * @return Span width in characters
     */
    [[nodiscard]] size_t width() const;

    /**
     * @brief Returns the 0-based line number of the span's start
     *
     * @return 0-based start line number
     */
    [[nodiscard]] size_t line() const;

    /**
     * @brief Returns the backing source of this span
     *
     * @return Shared pointer to the backing source
     */
    [[nodiscard]] const std::shared_ptr<Source>& source() const { return _source; }

    /**
     * @brief Returns the starting location of the span
     *
     * @return Start location (inclusive)
     */
    [[nodiscard]] Location start() const { return _start; }

    /**
     * @brief Returns the end location of the span
     *
     * @return End location (exclusive)
     */
    [[nodiscard]] Location end() const { return _end; }

private:
    std::shared_ptr<Source> _source;
    Location _start, _end;
};
} // namespace pretty_diagnostics

/**
 * @brief Streams a human-readable representation of a span for debugging
 *
 * @param os Output stream to write to
 * @param span Span to format
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const pretty_diagnostics::Span& span);

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
