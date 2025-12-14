#pragma once

#include <unordered_map>
#include <optional>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include "label.h"

namespace pretty_diagnostics {
class IReporterRenderer;

enum class Severity {
    Error,
    Warning,
    Info,
    Unknown,
};

class LineGroup {
public:
    LineGroup(size_t line_number, std::set<Label> labels);

    [[nodiscard]] auto& line_number() const { return _line_number; }

    [[nodiscard]] auto& labels() const { return _labels; }

    [[nodiscard]] auto& labels() { return _labels; }

private:
    std::set<Label> _labels;
    size_t _line_number;
};

class FileGroup {
public:
    using MappedLineGroups = std::map<size_t, LineGroup>;

public:
    FileGroup(const std::shared_ptr<Source>& source, MappedLineGroups line_groups);

    [[nodiscard]] auto& line_groups() const { return _line_groups; }

    [[nodiscard]] auto& line_groups() { return _line_groups; }

    [[nodiscard]] auto& source() const { return _source; }

private:
    std::shared_ptr<Source> _source;
    MappedLineGroups _line_groups;
};

class Report {
public:
    using MappedFileGroups = std::unordered_map<std::shared_ptr<Source>, FileGroup>;
    class Builder;

public:
    Report(std::string message, std::optional<std::string> code, Severity severity, MappedFileGroups file_groups,
           std::optional<std::string> note, std::optional<std::string> help);

    void render(IReporterRenderer& renderer, std::ostream& stream = std::cout) const;

    [[nodiscard]] auto& file_groups() const { return _file_groups; }

    [[nodiscard]] auto& file_groups() { return _file_groups; }

    [[nodiscard]] auto& severity() const { return _severity; }

    [[nodiscard]] auto& message() const { return _message; }

    [[nodiscard]] auto& note() const { return _note; }

    [[nodiscard]] auto& help() const { return _help; }

    [[nodiscard]] auto& code() const { return _code; }

private:
    std::optional<std::string> _code, _note, _help;
    MappedFileGroups _file_groups;
    std::string _message;
    Severity _severity;
};

class IReporterRenderer {
public:
    virtual ~IReporterRenderer() = default;

    virtual void render(const Severity& severity, std::ostream& stream) = 0;

    virtual void render(const Report& report, std::ostream& stream) = 0;

    virtual void render(const FileGroup& file_group, std::ostream& stream) = 0;

    virtual void render(const LineGroup& line_group, std::ostream& stream) = 0;
};

class Report::Builder {
public:
    Builder& severity(Severity severity);

    Builder& message(std::string message);

    Builder& code(std::string code);

    Builder& label(std::string text, Span span);

    Builder& note(std::string note);

    Builder& help(std::string help);

    [[nodiscard]] Report build() const;

private:
    std::optional<std::string> _message, _note, _help, _code;
    std::optional<Severity> _severity;
    MappedFileGroups _file_groups;
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
