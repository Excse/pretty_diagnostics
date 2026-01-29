#include "pretty_diagnostics/renderer.hpp"
#include "pretty_diagnostics/utils.hpp"

#include <unordered_map>
#include <ranges>
#include <vector>

using namespace pretty_diagnostics;

// TODO: Make this variable more dynamic
constexpr size_t MAX_TERMINAL_WIDTH = 80;
constexpr long MIN_TEXT_WRAP = 10;
constexpr long LINE_PADDING = 1;

TextRenderer::TextRenderer(const Report& report) {
    _padding = widest_line_number(report.file_groups(), LINE_PADDING) + 2;
    _snippet_width = static_cast<int>(_padding - 1);
    _whitespaces = std::string(_padding, ' ');
}

void TextRenderer::render(const Severity& severity, std::ostream& stream) {
    switch (severity) {
        case Severity::Error: {
            stream << "error";
            break;
        }
        case Severity::Warning: {
            stream << "warning";
            break;
        }
        case Severity::Info: {
            stream << "info";
            break;
        }
        case Severity::Unknown:
        default: {
            stream << "unknown";
            break;
        }
    }
}

/*
 *     header     ╶─┤ error[E1337]: Displaying a brief summary of what happened
 *     file_group ╶─┤    ╭╴resources/example╶─
 *     spacer     ╶─┤    ·
 *     line_group ╶┬┤  1 │ #include <stdio.h>
 *     w. labels   ╰┤    ·           ╰─────┴─▶ Relevant include to enable the usage of printf
 *     spacer     ╶─┤    ·
 *     context    ╶─┤  3 │ int main() {
 *     line_group ╶┬┤  4 │    printf("Hello World!\n");
 *     w. labels   ││    ·    ╰────┤ ╰──────────────┴─▶ This is the string that is getting printed
 *                 ││    ·         │                    to the console
 *                 ╰┤    ·         ╰─▶ And this is the function that actually makes the magic happen
 *     context    ╶─┤  5 │     return 0;
 *     spacer     ╶─┤    ·
 *     note       ╶┬┤    │ Note: This example showcases every little detail of the library, also with
 *                 ╰┤    │       the capability of line wrapping.
 *     help         │    │ Help: Visit https://github.com/Excse/pretty_diagnostics for more help.
 *     bottom     ╶─┤ ───╯
 */
void TextRenderer::render(const Report& report, std::ostream& stream) {
    const auto& groups = report.file_groups();

    this->render(report.severity(), stream);

    if (report.code().has_value()) stream << "[" << report.code().value() << "]";

    stream << ": " << report.message() << std::endl;

    for (auto it = groups.begin(); it != groups.end(); ++it) {
        const auto& [source, file_group] = *it;

        if (it == groups.begin()) stream << _whitespaces << "╭";
        else stream << _whitespaces << "├";

        stream << "╴" << source->path() << "╶─" << std::endl;

        if (it == groups.begin()) stream << _whitespaces << "·" << std::endl;

        render(file_group, stream);
    }

    if (report.note().has_value()) {
        const auto note_prefix = _whitespaces + "│ Note: ";
        const auto normal_prefix = _whitespaces + "│       ";

        const auto available_width = static_cast<long>(MAX_TERMINAL_WIDTH)
                                     - static_cast<long>(note_prefix.size());
        const auto max_text_width = static_cast<size_t>(std::max(MIN_TEXT_WRAP, available_width));

        const auto& text = report.note().value();
        const auto lines = wrap_text(text, max_text_width);
        stream << note_prefix << lines[0] << std::endl;

        for (size_t index = 1; index < lines.size(); ++index) {
            stream << normal_prefix << lines[index] << std::endl;
        }
    }

    if (report.help().has_value()) {
        const auto help_prefix = _whitespaces + "│ Help: ";
        const auto normal_prefix = _whitespaces + "│       ";

        const auto available_width = static_cast<long>(MAX_TERMINAL_WIDTH)
                                     - static_cast<long>(help_prefix.size());
        const auto max_text_width = static_cast<size_t>(std::max(MIN_TEXT_WRAP, available_width));

        const auto& text = report.help().value();
        const auto lines = wrap_text(text, max_text_width);
        stream << help_prefix << lines[0] << std::endl;

        for (size_t index = 1; index < lines.size(); ++index) {
            stream << normal_prefix << lines[index] << std::endl;
        }
    }

    stream << _whitespaces << "╯" << std::endl;
}

void TextRenderer::render(const FileGroup& file_group, std::ostream& stream) {
    const auto max_line = static_cast<long>(file_group.source()->line_count());
    const auto& line_groups = file_group.line_groups();

    for (auto it = line_groups.begin(); it != line_groups.end(); ++it) {
        const auto& label_group = it->second;
        const auto line_number = label_group.line_number();

        const auto min_padded_line = std::max(0L, static_cast<long>(line_number) - LINE_PADDING);
        const auto max_padded_line = std::min(max_line - 1, static_cast<long>(line_number) + LINE_PADDING);

        // Calculate if we need a spacer before this group (if it's not the first and doesn't overlap with previous)
        if (it != line_groups.begin()) {
            const auto &previous_group = std::prev(it)->second;

            const auto prev_line_number = previous_group.line_number();
            const auto prev_max_padded = std::min(max_line - 1, static_cast<long>(prev_line_number) + LINE_PADDING);

            if (min_padded_line > prev_max_padded + 1) {
                stream << _whitespaces << "· " << std::endl;
            }
        }

        for (size_t padded_line = min_padded_line; padded_line <= static_cast<size_t>(max_padded_line); ++padded_line) {
            // Avoid double-rendering lines if they are part of multiple groups
            if (it != line_groups.begin()) {
                const auto &previous_group = std::prev(it)->second;

                const auto prev_line_number = previous_group.line_number();
                const auto prev_max_padded = std::min(max_line - 1, static_cast<long>(prev_line_number) + LINE_PADDING);

                if (padded_line <= prev_max_padded) {
                    if (padded_line == line_number) TextRenderer::render(label_group, stream);
                    continue;
                }
            }

            const auto line = file_group.source()->line(padded_line);

            stream << std::setw(static_cast<int>(_snippet_width)) << padded_line + 1 << " │ " << line << std::endl;

            if (padded_line == line_number) TextRenderer::render(label_group, stream);
        }
    }

    stream << _whitespaces << "· " << std::endl;
}

void TextRenderer::render(const LineGroup& line_group, std::ostream& stream) {
    const auto& labels = line_group.labels();

    for (auto last_it = labels.rbegin(); last_it != labels.rend(); ++last_it) {
        const auto& label = *last_it;
        const auto end_column = label.span().end().column();

        // This equation consists of the following parts:
        // [prefix][padding until label end][arrow][wrapped_text]
        //  (padding + 1)      -> "  · " (the dynamic prefix)
        //  (end_column + 4)   -> "┴─▶ " (4 characters have to be drawn at end_column)
        //  MAX_TERMINAL_WIDTH -> a (for now) static variable that determines the terminal width
        const auto available_width = static_cast<long>(MAX_TERMINAL_WIDTH)
                                     - static_cast<long>(end_column + 4)
                                     - static_cast<long>(_padding + 1);
        const auto max_text_width = static_cast<size_t>(std::max(MIN_TEXT_WRAP, available_width));
        const auto text_lines = wrap_text(label.text(), max_text_width);

        for (size_t text_index = 0; text_index < text_lines.size(); ++text_index) {
            stream << _whitespaces << "· ";

            size_t current_column = 0;
            for (auto other_it = labels.begin(); other_it != std::prev(last_it.base()); ++other_it) {
                const auto& other_label = *other_it;
                const auto& finished = render(other_label, stream, text_lines, text_index, false, current_column);
                current_column = finished + 1;
            }

            render(label, stream, text_lines, text_index, true, current_column);
            stream << std::endl;
        }
    }
}

size_t TextRenderer::render(const Label& label, std::ostream& stream,
                            const std::vector<std::string>& text_lines, const size_t text_index,
                            const bool active_render, const size_t column_start) {
    const auto start_column = label.span().start().column();
    const auto end_column = label.span().end().column();

    const auto& current_text = text_lines[text_index];

    size_t column = column_start;
    for (; column < end_column; column++) {
        if (column == end_column - 1) {
            if (!active_render) {
                stream << "│";
                break;
            }

            if (text_index == 0) {
                if (start_column == end_column - 1) stream << "╰─▶ ";
                else stream << "┴─▶ ";
            } else {
                // We need to pad with spaces equal to the visual width of "┴─▶ " or "╰─▶ ", which is 4.
                stream << "    ";
            }

            stream << current_text;
            break;
        }

        if (column == start_column) {
            if (active_render && text_index == 0) stream << "╰";
            else if (!active_render) stream << "│";
            else stream << " ";
        } else if (column > start_column && column < end_column) {
            if (active_render && text_index == 0) stream << "─";
            else stream << " ";
        } else {
            stream << " ";
        }
    }

    return column;
}

size_t TextRenderer::widest_line_number(const Report::MappedFileGroups& groups, const size_t padding) {
    long line = 0;

    for (const auto& group : groups | std::views::values) {
        if (group.line_groups().empty()) continue;
        const auto biggest_line = group.line_groups().rbegin()->first;
        if (biggest_line > static_cast<size_t>(line)) line = static_cast<long>(biggest_line);
    }

    // line + 1 is the 1-based line number for display
    const auto line_string = std::to_string(line + 1 + static_cast<long>(padding));
    const auto width = visual_width(line_string);

    return width;
}

std::vector<std::string> TextRenderer::wrap_text(const std::string& text, const size_t max_width) {
    std::vector<std::string> lines;
    std::istringstream line_stream(text);
    std::string current_paragraph;

    // Always read an entire paragraph at once to ensure \n still works.
    while (std::getline(line_stream, current_paragraph, '\n')) {
        // If the pattern \n\n occured, just add an empty line.
        if (current_paragraph.empty()) {
            lines.emplace_back("");
            continue;
        }

        std::istringstream word_stream(current_paragraph);

        std::string current_line;
        std::string current_word;
        size_t position = 0;

        // Go over each word and test if it fits in the current line or a new one needs to be constructed.
        while (word_stream >> current_word) {
            // Calculates the word length. If the line is empty, no whitespace has to be added. Otherwise, it is necessary
            // for concatenation of words.
            const std::string prefix = current_line.empty() ? "" : " ";
            const size_t word_length = visual_width(current_word) + visual_width(prefix);

            // If the word itself doesn't fit into the max width, hard-break it.
            if (word_length > (max_width - position)) {
                // If the current line is non-empty add it to the lines.
                if (!current_line.empty()) lines.push_back(current_line);

                // If the one single word is larger than the entire max width, hard-split it until it fits.
                while (visual_width(current_word) > max_width) {
                    // Extract exactly one line worth out of the current word.
                    auto byte_index = from_visual_column(current_word, max_width);
                    const auto& substring = current_word.substr(0, byte_index);
                    lines.push_back(substring);

                    // Remove the added substring part from the current word.
                    current_word = current_word.substr(byte_index);
                }

                // Set the current line to be the word and reset the position pointer.
                current_line = current_word;
                position = visual_width(current_word);
            } else {
                // Add the separator and word to the current line and adjust the position.
                current_line += prefix + current_word;
                position += word_length;
            }
        }

        // Add the remaining line to the vector.
        if (!current_line.empty()) lines.push_back(current_line);
    }

    return lines;
}

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
