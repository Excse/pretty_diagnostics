#include "pretty_diagnostics/renderer.hpp"
#include "pretty_diagnostics/utils.hpp"

#include <ranges>
#include <unordered_map>
#include <vector>

using namespace pretty_diagnostics;

// TODO: Make this variable more dynamic
constexpr size_t MAX_TERMINAL_WIDTH = 80;
constexpr long MIN_TEXT_WRAP = 10;
constexpr long LINE_PADDING = 1;

GlyphSet Glyphs::Unicode() {
    return {
        .corner_top_left = "╭",
        .corner_bottom_right = "╯",
        .tee_right = "├",
        .cap_left = "╴",
        .cap_right = "╶─",
        .line_vertical = "│",
        .line_horizontal = "─",
        .label_start = "╰",
        .label_end = "┴",
        .filler = "·",
        .arrow_right = "▶",
    };
}

GlyphSet Glyphs::Ascii() {
    return {
        .corner_top_left = "+",
        .corner_bottom_right = "+",
        .tee_right = "├",
        .cap_left = "-",
        .cap_right = "--",
        .line_vertical = "|",
        .line_horizontal = "~",
        .label_start = "^",
        .label_end = "^",
        .filler = ".",
        .arrow_right = ">",
    };
}

TextRenderer::TextRenderer(const Report& report, Config config) : _config(std::move(config)) {
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
    const auto& file_groups = report.file_groups();

    this->render(report.severity(), stream);

    if (report.code().has_value()) {
        stream << "[" << report.code().value() << "]";
    }

    stream << ": ";

    const auto message_wrapped_padding = stream.tellp();
    const auto message_available_width = static_cast<long>(MAX_TERMINAL_WIDTH) - message_wrapped_padding;
    const auto message_wrapped_prefix = std::string(message_wrapped_padding, ' ');
    print_wrapped_text(report.message(), message_wrapped_prefix, message_available_width, stream);

    for (auto it = file_groups.begin(); it != file_groups.end(); ++it) {
        const auto& [source, file_group] = *it;

        if (it == file_groups.begin()) {
            stream << _whitespaces << _config.glyphs.corner_top_left;
        } else {
            stream << _whitespaces << _config.glyphs.tee_right;
        }

        stream << _config.glyphs.cap_left << source->path() << _config.glyphs.cap_right << "\n";

        if (it == file_groups.begin()) {
            stream << _whitespaces << _config.glyphs.filler << "\n";
        }

        render(file_group, stream);
    }

    if (report.note().has_value()) {
        const auto note_prefix = _whitespaces + _config.glyphs.line_vertical + " Note: ";
        const auto note_wrapped_prefix = _whitespaces + _config.glyphs.line_vertical + "       ";

        const auto note_wrapped_padding = visual_width(note_wrapped_prefix);
        const auto note_available_width = static_cast<long>(MAX_TERMINAL_WIDTH) - note_wrapped_padding;

        stream << note_prefix;
        print_wrapped_text(report.note().value(), note_wrapped_prefix, note_available_width, stream);
    }

    if (report.help().has_value()) {
        const auto help_prefix = _whitespaces + _config.glyphs.line_vertical + " Help: ";
        const auto help_wrapped_prefix = _whitespaces + _config.glyphs.line_vertical + "       ";

        const auto help_wrapped_padding = visual_width(help_wrapped_prefix);
        const auto help_available_width = static_cast<long>(MAX_TERMINAL_WIDTH) - help_wrapped_padding;

        stream << help_prefix;
        print_wrapped_text(report.help().value(), help_wrapped_prefix, help_available_width, stream);
    }

    stream << _whitespaces << _config.glyphs.corner_bottom_right << "\n";
}

void TextRenderer::render(const FileGroup& file_group, std::ostream& stream) {
    const auto max_line = static_cast<long>(file_group.source()->line_count());
    const auto& line_groups = file_group.line_groups();

    long max_rendered_line = -1;
    for (auto it = line_groups.begin(); it != line_groups.end(); ++it) {
        const auto& label_group = it->second;

        const auto next_line = (it == std::prev(line_groups.end())) ? max_line : static_cast<long>(std::next(it)->second.line_number());
        const auto prev_line = (it == line_groups.begin()) ? -1 : static_cast<long>(std::prev(it)->second.line_number());

        const auto current_line = static_cast<long>(label_group.line_number());

        const auto max_padded_line = min(max_line - 1, next_line - 1, current_line + LINE_PADDING);
        const auto min_padded_line = max(0L, prev_line + 1, current_line - LINE_PADDING);

        if (it != line_groups.begin() && min_padded_line > max_rendered_line + 1) {
            stream << _whitespaces << _config.glyphs.filler << " \n";
        }

        for (long line = min_padded_line; line <= max_padded_line; ++line) {
            const bool source_line_needed = line > max_rendered_line;
            const bool render_label_here = line == current_line;

            if (source_line_needed) {
                const auto line_text = file_group.source()->line(line);
                stream << std::setw(static_cast<int>(_snippet_width))
                       << line + 1 << " "
                       << _config.glyphs.line_vertical << " "
                       << line_text << '\n';
                max_rendered_line = line;
            }

            if (render_label_here) {
                TextRenderer::render(label_group, stream);
            }
        }
    }

    stream << _whitespaces << _config.glyphs.filler << " \n";
}

void TextRenderer::render(const LineGroup& line_group, std::ostream& stream) {
    const auto& labels = line_group.labels();

    for (auto active_it = labels.rbegin(); active_it != labels.rend(); ++active_it) {
        const auto& active_label = *active_it;

        // This equation consists of the following parts:
        // [prefix][padding until label end][arrow][wrapped_text]
        //  (padding + 1)      -> "  · " (the dynamic prefix)
        //  (end_column + 4)   -> "┴─▶ " (4 characters have to be drawn at end_column)
        //  MAX_TERMINAL_WIDTH -> a (for now) static variable that determines the terminal width
        const auto end_column = active_label.span().end().column();
        const auto available_width = static_cast<long>(MAX_TERMINAL_WIDTH) - static_cast<long>(end_column + 4) - static_cast<long>(_padding + 1);

        const auto max_text_width = static_cast<size_t>(std::max(MIN_TEXT_WRAP, available_width));
        const auto text_lines = wrap_text(active_label.text(), max_text_width);

        for (size_t text_index = 0; text_index < text_lines.size(); ++text_index) {
            stream << _whitespaces << _config.glyphs.filler << " ";

            size_t current_column = 0;
            for (auto inactive_it = labels.begin(); inactive_it != std::prev(active_it.base()); ++inactive_it) {
                const auto& inactive_label = *inactive_it;
                render(inactive_label, stream, text_lines, text_index, false, current_column);
                current_column = inactive_label.span().end().column();
            }

            render(active_label, stream, text_lines, text_index, true, current_column);
            stream << "\n";
        }
    }
}

void TextRenderer::render(const Label& label, std::ostream& stream, const std::vector<std::string>& text_lines, const size_t text_index,
                          const bool active_render, const size_t column_start) const {
    const auto start = label.span().start().column();
    const auto end = label.span().end().column();
    const auto is_first_line = (text_index == 0);

    for (size_t column = column_start; column < end - 1; column++) {
        if (column == start) {
            if (active_render) {
                if (is_first_line) {
                    stream << _config.glyphs.label_start;
                } else {
                    stream << " ";
                }
            } else {
                stream << _config.glyphs.line_vertical;
            }
        } else if (column > start) {
            if (active_render && is_first_line) {
                stream << _config.glyphs.line_horizontal;
            } else {
                stream << " ";
            }
        } else {
            stream << " ";
        }
    }

    if (!active_render) {
        stream << _config.glyphs.line_vertical;
        return;
    }

    if (is_first_line) {
        if (start == end - 1) {
            stream << _config.glyphs.label_start;
        } else {
            stream << _config.glyphs.label_end;
        }

        stream << _config.glyphs.line_horizontal << _config.glyphs.arrow_right << " ";
    } else {
        // We need to pad with spaces equal to the visual width of "┴─▶ " or "╰─▶ ", which is 4.
        stream << "    ";
    }

    const auto& text = text_lines[text_index];
    stream << text;
}

size_t TextRenderer::widest_line_number(const Report::MappedFileGroups& groups, const size_t padding) {
    size_t max_line = 0;

    for (const auto& group : groups | std::views::values) {
        auto& lines = group.line_groups();
        if (lines.empty()) continue;

        // line_groups() is ordered; last key is the largest line number
        max_line = std::max(max_line, lines.rbegin()->first);
    }

    // Convert to the 1-based display line number and apply padding
    const size_t display_line = max_line + 1 + padding;

    return visual_width(std::to_string(display_line));
}

std::vector<std::string> TextRenderer::wrap_text(const std::string& text, const size_t max_width) {
    std::vector<std::string> lines;
    if (text.empty()) return lines;

    std::istringstream line_stream(text);
    std::string current_paragraph;

    // Always read an entire paragraph at once to ensure \n still works.
    while (std::getline(line_stream, current_paragraph, '\n')) {
        // If the pattern \n\n occured, just add an empty line.
        if (current_paragraph.empty()) {
            lines.emplace_back("");
            continue;
        }

        size_t visual_position = 0;
        size_t byte_position = 0;
        std::string current_line;

        const auto add_chunk = [&](std::string chunk) {
            if (chunk.empty()) return;

            const size_t chunk_width = visual_width(chunk);

            // If the word itself doesn't fit into the max width, hard-break it.
            if (chunk_width > (max_width - visual_position)) {
                // If the current line is non-empty add it to the lines.
                if (!current_line.empty()) {
                    lines.push_back(current_line);
                    current_line.clear();
                    visual_position = 0;
                }

                // If the one single word is larger than the entire max width, hard-split it until it fits.
                while (true) {
                    const auto visua = visual_width(chunk);
                    if (visua <= max_width) break;

                    // Extract exactly one line worth out of the current word.
                    const auto byte_index = from_visual_column(chunk, max_width);
                    if (byte_index == 0) break;

                    const auto& substring = chunk.substr(0, byte_index);
                    lines.push_back(substring);

                    // Remove the added substring part from the current word.
                    chunk = chunk.substr(byte_index);
                }

                current_line = chunk;
                visual_position = visual_width(chunk);
            } else {
                current_line += chunk;
                visual_position += chunk_width;
            }
        };

        while (byte_position < current_paragraph.size()) {
            const auto word_start = byte_position;
            while (byte_position < current_paragraph.size()) {
                const auto current_char = current_paragraph[byte_position];
                if (std::isspace(current_char)) break;

                const auto [_, byte_count] = get_visual_char(current_paragraph, byte_position);
                byte_position += byte_count;
            }

            const auto word = current_paragraph.substr(word_start, byte_position - word_start);
            add_chunk(word);

            const auto space_start = byte_position;
            while (byte_position < current_paragraph.size()) {
                const auto current_char = current_paragraph[byte_position];
                if (!std::isspace(current_char)) break;

                const auto [_, byte_count] = get_visual_char(current_paragraph, byte_position);
                byte_position += byte_count;
            }

            const auto space = current_paragraph.substr(space_start, byte_position - space_start);
            add_chunk(space);
        }

        // Add the remaining line to the vector.
        if (!current_line.empty()) lines.push_back(current_line);
    }

    return lines;
}

void TextRenderer::print_wrapped_text(const std::string& text, const std::string& wrapped_prefix, const size_t max_width, std::ostream& stream) {
    const auto lines = wrap_text(text, max_width);
    if (lines.empty()) {
        stream << "\n";
        return;
    }

    stream << lines.front() << "\n";

    for (const auto& line : lines | std::views::drop(1)) {
        stream << wrapped_prefix << line << "\n";
    }
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
