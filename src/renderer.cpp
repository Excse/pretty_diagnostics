#include "pretty_diagnostics/renderer.h"

#include <unordered_map>
#include <ranges>
#include <vector>

using namespace pretty_diagnostics;

// TODO: Make this variable more dynamic
constexpr size_t MAX_TERMINAL_WIDTH = 80;
constexpr size_t LINE_PADDING = 3;

void TextRenderer::render(const Severity &severity, std::ostream &stream) const {
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
 *     header  ╶─┤ error[E1337]: Displaying a brief summary of what happened
 *     group   ╶─┤    ╭╴resources/example╶─
 *     filler  ╶─┤    ·
 *     snippet ╶─┤  1 │ #include <stdio.h>
 *     label   ╶─┤    ·           ╰─────┴─▶ Relevant include to enable the usage of printf
 *     spacer  ╶─┤    ⋯
 *     filler  ╶─┤    ·
 *     snippet ╶─┤  3 │ int main() {
 *     snippet ╶─┤  4 │    printf("Hello World!\n");
 *              ╭┤    ·    ╰────┤ ╰──────────────┴─▶ This is the string that is getting printed
 *     label   ╶┤│    ·         │                    to the console
 *              ╰┤    ·         ╰─▶ And this is the function that actually makes the magic happen
 *     snippet ╶─┤  5 │     return 0;
 *     spacer  ╶─┤    ⋯
 *     note    ╶┬┤    │ Note: This example showcases every little detail of the library, also with
 *              ╰┤    │       the capability of line wrapping.
 *     help      │    │ Help: Visit https://github.com/Excse/pretty_diagnostics for more help.
 *     bottom ╶─╴│ ───╯
 */
void TextRenderer::render(const Report &report, std::ostream &stream) const {
    this->render(report.severity(), stream);

    if (report.code().has_value()) stream << "[" << report.code().value() << "]";

    stream << ": " << report.message() << std::endl;

    const auto &groups = report.label_groups();

    const auto padding = widest_line_number(groups, LINE_PADDING) + 2;
    const auto &snippet_width = static_cast<int>(padding - 1);
    const auto whitespaces = std::string(padding, ' ');

    for (auto group_it = groups.begin(); group_it != groups.end(); ++group_it) {
        const auto &[source, labels] = *group_it;

        if (group_it == groups.begin()) stream << whitespaces << "╭";
        else                            stream << whitespaces << "├";

        stream << "╴" << source->path() << "╶" << std::endl;

        for (const auto &label: labels) {
            const auto line = source->line(label.span().start());
            const auto end_column = label.span().end().column();
            const auto line_number = label.span().line();

            stream << std::setw(snippet_width) << line_number << " │ " << line << std::endl;

            // TODO: Figure a way out to make this more dynamic
            // This equation consists of the following parts:
            //  (padding + 1)      -> "  · " (the dynamic prefix)
            //  (end_column - 4)   -> "┴─▶ " (4 characters have to be drawn at end_column)
            //  MAX_TERMINAL_WIDTH -> a (for now) static variable that determines the terminal width
            const auto max_text_width = MAX_TERMINAL_WIDTH - (end_column - 4) - (padding + 1);
            const auto text_lines = wrap_text(label.text(), max_text_width);

            for (size_t text_index = 0; text_index < text_lines.size(); ++text_index) {
                stream << whitespaces << "· ";
                render(label, stream, text_lines, text_index, 0);
                stream << std::endl;
            }
        }

        if (std::next(group_it) == groups.end()) stream << whitespaces << "╯" << std::endl;
    }
}

void TextRenderer::render(const Label &label, std::ostream &stream,
                          const std::vector<std::string> &text_lines, const size_t text_index,
                          const size_t column_start) {
    const auto start_column = label.span().start().column();
    const auto end_column = label.span().end().column();

    const auto &current_text = text_lines[text_index];
    const auto is_first_line = (text_index == 0);

    for (size_t column = column_start; column <= end_column; ++column) {
        if (column == end_column - 1) {
            if (is_first_line) stream << "┴─▶ ";
            else               stream << "    ";

            stream << current_text;
            break;
        }

        if (!is_first_line) {
            stream << " ";
        } else if (column == start_column) {
            stream << "╰";
        } else if (column > start_column && column < end_column) {
            stream << "─";
        } else {
            stream << " ";
        }
    }
}

size_t TextRenderer::widest_line_number(const Report::GroupedLabels &groups, const size_t padding) {
    size_t line = 0;

    for (const auto &labels: groups | std::views::values) {
        for (const auto &label: labels) {
            const size_t line_number = label.span().line() + padding;
            if (line_number > line) line = line_number;
        }
    }

    return std::to_string(line).size();
}

std::vector<std::string> TextRenderer::wrap_text(const std::string &text, const size_t max_width) {
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
            const size_t word_length = current_word.length() + prefix.length();

            // If the word itself doesn't fit into the max width, hard-break it.
            if (word_length > (max_width - position)) {
                // If the current line is non-empty add it to the lines.
                if (!current_line.empty()) lines.push_back(current_line);

                // If the one single word is larger than the entire max width, hard-split it until it fits.
                while (current_word.length() > max_width) {
                    // Extract exactly one line worth out of the current word.
                    const auto &substring = current_word.substr(0, max_width);
                    lines.push_back(substring);

                    // Remove the added substring part from the current word.
                    current_word = current_word.substr(max_width);
                }

                // Set the current line to be the word and reset the position pointer.
                current_line = current_word;
                position = current_word.length();
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
