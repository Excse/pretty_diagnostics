#pragma once

#include <unordered_map>
#include <optional>
#include <iostream>
#include <vector>
#include <string>

#include "label.h"

namespace pretty_diagnostics {

enum class Severity {
    Error,
    Warning,
    Info,
    Unknown,
};

class IReporterRenderer;

class Report {
public:
    using GroupedLabels = std::unordered_map<std::shared_ptr<Source>, std::vector<Label>>;
    class Builder;

public:
    Report(std::string message, std::optional<std::string> code, Severity severity, GroupedLabels label_groups);

    void render(const IReporterRenderer &renderer, std::ostream &stream = std::cout) const;

    [[nodiscard]] auto &label_groups() const { return _label_groups; }

    [[nodiscard]] auto &severity() const { return _severity; }

    [[nodiscard]] auto &message() const { return _message; }

    [[nodiscard]] auto &code() const { return _code; }

private:
    std::optional<std::string> _code;
    GroupedLabels _label_groups;
    std::string _message;
    Severity _severity;
};

class IReporterRenderer {
public:
    virtual ~IReporterRenderer() = default;

    virtual void render(const Severity &severity, std::ostream &stream) const = 0;

    virtual void render(const Report &report, std::ostream &stream) const = 0;
};

class Report::Builder {
public:
    Builder &severity(Severity severity);

    Builder &message(std::string message);

    Builder &code(std::string code);

    Builder &label(std::string text, Span span);

    [[nodiscard]] Report build() const;

private:
    std::optional<std::string> _message;
    std::optional<Severity> _severity;
    std::optional<std::string> _code;
    GroupedLabels _labels;
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