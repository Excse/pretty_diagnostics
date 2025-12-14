#pragma once

#include "report.h"

namespace pretty_diagnostics {
class TextRenderer final : public IReporterRenderer {
public:
    explicit TextRenderer(const Report& report);

    void render(const Severity& severity, std::ostream& stream) override;

    void render(const Report& report, std::ostream& stream) override;

    void render(const FileGroup& file_group, std::ostream& stream) override;

    void render(const LineGroup& line_group, std::ostream& stream) override;

    static size_t render(const Label& label, std::ostream& stream,
                         const std::vector<std::string>& text_lines, size_t text_index,
                         bool active_render, size_t column_start = 0);

    [[nodiscard]] static size_t widest_line_number(const Report::MappedFileGroups& groups, size_t padding);

    [[nodiscard]] static std::vector<std::string> wrap_text(const std::string& text, size_t max_width);

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