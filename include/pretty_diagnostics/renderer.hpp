#pragma once

#include <vector>

#include "report.hpp"

namespace pretty_diagnostics {
/**
 * @brief A plain-text renderer for diagnostic `Report`s
 *
 * Produces a clean, human-friendly multi-line output similar to compilers
 */
class TextRenderer final : public IReporterRenderer {
public:
    /**
     * @brief Initializes the renderer with a reference layout taken from a `Report`
     *
     * @param report Report instance used to derive initial layout parameters
     */
    explicit TextRenderer(const Report& report);

    /**
     * @brief Renders just the severity label to the stream
     *
     * @param severity Severity to render (e.g., error, warning)
     * @param stream Output stream to write to
     */
    void render(const Severity& severity, std::ostream& stream) override;

    /**
     * @brief Renders an entire report to the stream
     *
     * @param report Report to be rendered
     * @param stream Output stream to write to
     */
    void render(const Report& report, std::ostream& stream) override;

    /**
     * @brief Renders a single file group to the stream
     *
     * @param file_group File group to render
     * @param stream Output stream to write to
     */
    void render(const FileGroup& file_group, std::ostream& stream) override;

    /**
     * @brief Renders a single line group to the stream
     *
     * @param line_group Line group to render
     * @param stream Output stream to write to
     */
    void render(const LineGroup& line_group, std::ostream& stream) override;

    /**
     * @brief Renders an individual label, optionally in active mode to draw the actual
     *        span arrows
     *
     * @param label Label to render
     * @param stream Output stream to write to
     * @param text_lines Pre-wrapped lines of the label text
     * @param text_index Index into @p text_lines to start rendering from
     * @param active_render Whether to draw guides/arrows for the label
     * @param column_start Optional starting column for rendering (0-based)
     *
     * @return Next text index to continue rendering wrapped text
     */
    static size_t render(const Label& label, std::ostream& stream,
                         const std::vector<std::string>& text_lines, size_t text_index,
                         bool active_render, size_t column_start = 0);

    /**
     * @brief Computes the width of the widest line number across groups, plus padding
     *
     * @param groups Mapped file groups used for rendering
     * @param padding Extra characters to add to the computed width
     *
     * @return Total width for the line-number column
     */
    [[nodiscard]] static size_t widest_line_number(const Report::MappedFileGroups& groups, size_t padding);

    /**
     * @brief Wraps the given text to lines no longer than `max_width` characters
     *
     * @param text Text to wrap
     * @param max_width Maximum line width
     *
     * @return Wrapped text as a vector of lines
     */
    [[nodiscard]] static std::vector<std::string> wrap_text(const std::string& text, size_t max_width);

    /**
     * @brief Prints the wrapped text into lines no longer than `max_width` characters and adds a prefix to
     *        wrapped lines
     *
     * @param text Text to wrap
     * @param wrapped_prefix The prefix for wrapped lines
     * @param max_width Maximum line width
     * @param stream  Output stream to write to
     */
    static void print_wrapped_text(const std::string &text, const std::string& wrapped_prefix, size_t max_width, std::ostream &stream);

private:
    size_t _padding, _snippet_width;
    std::string _whitespaces;
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
