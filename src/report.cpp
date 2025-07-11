#include "pretty_diagnostics/report.h"

#include <stdexcept>

using namespace pretty_diagnostics;

LineGroup::LineGroup(const size_t line_number, std::set<Label> labels)
    : _labels(std::move(labels)), _line_number(line_number) {
}

FileGroup::FileGroup(const std::shared_ptr<Source> &source, MappedLineGroups line_groups)
    : _source(source), _line_groups(std::move(line_groups)) {
}

Report::Report(std::string message, std::optional<std::string> code, const Severity severity,
               MappedFileGroups file_groups, std::optional<std::string> note, std::optional<std::string> help)
    : _code(std::move(code)), _note(std::move(note)), _help(std::move(help)),
      _file_groups(std::move(file_groups)), _message(std::move(message)), _severity(severity) {
}

void Report::render(IReporterRenderer &renderer, std::ostream &stream) const {
    renderer.render(*this, stream);
}

Report::Builder &Report::Builder::severity(Severity severity) {
    _severity = severity;
    return *this;
}

Report::Builder &Report::Builder::message(std::string message) {
    _message = std::move(message);
    return *this;
}

Report::Builder &Report::Builder::code(std::string code) {
    _code = std::move(code);
    return *this;
}

Report::Builder &Report::Builder::label(std::string text, Span span) {
    if (text.empty()) throw std::runtime_error("Report::Builder::label(): label text is empty");

    auto &file_group = _file_groups.try_emplace(span.source(), FileGroup(span.source(), FileGroup::MappedLineGroups())).first->second;
    auto &line_group = file_group.line_groups().try_emplace(span.line(), LineGroup(span.line(), {})).first->second;

    for (const auto &label : line_group.labels()) {
        if (!label.span().intersects(span)) continue;

        throw std::runtime_error("Report::Builder::label(): there is an intersection with a different label");
    }

    line_group.labels().insert(Label(std::move(text), std::move(span)));

    return *this;
}

Report::Builder & Report::Builder::note(std::string note) {
    _note = std::move(note);
    return *this;
}

Report::Builder & Report::Builder::help(std::string help) {
    _help = std::move(help);
    return *this;
}

Report Report::Builder::build() const {
    if (!_message.has_value()) {
        throw std::runtime_error("Report::Builder::build(): message is not set");
    }

    return {
        _message.value(),
        _code,
        _severity.value_or(Severity::Error),
        _file_groups,
        _note,
        _help,
    };
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
