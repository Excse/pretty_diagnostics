#include "pretty_diagnostics/renderer.h"

#include <ranges>
#include <unordered_map>
#include <vector>

using namespace pretty_diagnostics;

constexpr size_t LINE_PADDING = 3;

size_t widest_line_number(const Report::GroupedLabels &groups, const size_t padding) {
    size_t line = 0;

    for (const auto &labels: groups | std::views::values) {
        for (const auto &label: labels) {
            const size_t line_number = label.span().line() + padding;
            if (line_number > line) line = line_number;
        }
    }

    return std::to_string(line).size();
}

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
 *              ╭┤    ·    ╰────┤ ╰──────────────┴─▶ This is the string that is getting
 *     label   ╶┤│    ·         │                    printed to the console
 *              ╰┤    ·         ╰─▶ And this is the function actually makes the magic happen
 *     snippet ╶─┤  5 │     return 0;
 *     spacer  ╶─┤    ⋯
 *     note    ╶┬┤    │ Note: This example showcases every little detail of the library, also
 *              ╰┤    │       with the capability of line wrapping.
 *     help      │    │ Help: Visit https://github.com/Excse/pretty_diagnostics for more help.
 *     bottom ╶─╴│ ───╯
 */
void TextRenderer::render(const Report &report, std::ostream &stream) const {
    this->render(report.severity(), stream);

    if (report.code().has_value()) stream << "[" << report.code().value() << "]";

    stream << ": " << report.message() << std::endl;

    const auto &groups = report.label_groups();

    const auto padding = widest_line_number(groups, LINE_PADDING) + 2;
    const auto whitespaces = std::string(padding, ' ');
    const auto last_padding = std::wstring(padding, L'─');

    for (auto it = groups.begin(); it != groups.end(); ++it) {
        const auto &[source, labels] = *it;

        if (it == groups.begin())          stream << whitespaces << "╭";
        else                               stream << whitespaces << "├";

        stream << "╴" << source->path() << "╶" << std::endl;

        if (std::next(it) == groups.end()) stream << last_padding << "╯" << std::endl;
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
