#pragma once

#include <unordered_map>
#include <optional>
#include <iostream>
#include <string>
#include <map>
#include <set>

#include "label.hpp"

namespace pretty_diagnostics {
class IReporterRenderer;

/**
 * @brief Indicates the importance of a diagnostic
 */
enum class Severity {
    Error,   ///< Serious problem that usually prevents progress
    Warning, ///< Suspicious or suboptimal situation
    Info,    ///< Informational message
    Unknown, ///< Unspecified or not set
};

/**
 * @brief A set of labels that belong to the same 0-based line number
 */
class LineGroup {
public:
    /**
     * @brief Constructs a group for a single line and its labels
     *
     * @param line_number 0-based line number
     * @param labels Labels associated with this line
     */
    LineGroup(size_t line_number, std::set<Label> labels);

    /**
     * @brief Returns the 0-based line number
     *
     * @return 0-based line number of this group
     */
    [[nodiscard]] size_t line_number() const { return _line_number; }

    /**
     * @brief Returns the set of labels for this line
     *
     * @return Const reference to labels
     */
    [[nodiscard]] const std::set<Label>& labels() const { return _labels; }

    /**
     * @brief Returns the set of labels for this line
     *
     * @return Reference to labels
     */
    [[nodiscard]] std::set<Label>& labels() { return _labels; }

private:
    std::set<Label> _labels;
    size_t _line_number;
};

/**
 * @brief Groups `LineGroup`s belonging to the same `Source`
 */
class FileGroup {
public:
    using MappedLineGroups = std::map<size_t, LineGroup>;

public:
    /**
     * @brief Constructs a group for a source file with its line groups
     *
     * @param source Backing source for the group
     * @param line_groups Mapping from line number to line groups
     */
    FileGroup(const std::shared_ptr<Source>& source, MappedLineGroups line_groups);

    /**
     * @brief Returns the map of line groups
     *
     * @return Reference to mapped line groups
     */
    [[nodiscard]] const MappedLineGroups& line_groups() const { return _line_groups; }

    /**
     * @brief Returns the map of line groups
     *
     * @return Reference to mapped line groups
     */
    [[nodiscard]] MappedLineGroups& line_groups() { return _line_groups; }

    /**
     * @brief Returns the source this file group refers to
     *
     * @return Shared pointer to the backing source
     */
    [[nodiscard]] const std::shared_ptr<Source>& source() const { return _source; }

private:
    std::shared_ptr<Source> _source;
    MappedLineGroups _line_groups;
};

/**
 * @brief Represents a fully constructed diagnostic report to be rendered
 */
class Report {
public:
    using MappedFileGroups = std::unordered_map<std::shared_ptr<Source>, FileGroup>;
    class Builder;

public:
    /**
     * @brief Constructs a report
     *
     * @param message Primary diagnostic message
     * @param code Optional error code or identifier
     * @param severity Diagnostic severity
     * @param file_groups Mapping from sources to their file group
     * @param note Optional note for additional context
     * @param help Optional help text with suggestions
     */
    Report(std::string message, std::optional<std::string> code, Severity severity, MappedFileGroups file_groups,
           std::optional<std::string> note, std::optional<std::string> help);

    /**
     * @brief Renders the report using the provided renderer to the output stream
     *
     * @param renderer Renderer implementation
     * @param stream Output stream to write to (defaults to std::cout)
     */
    void render(IReporterRenderer& renderer, std::ostream& stream = std::cout) const;

    /**
     * @brief Returns the mapping of sources to file groups
     *
     * @return Reference to mapped file groups
     */
    [[nodiscard]] const MappedFileGroups& file_groups() const { return _file_groups; }

    /**
     * @brief Returns the mapping of sources to file groups
     *
     * @return Reference to mapped file groups
     */
    [[nodiscard]] MappedFileGroups& file_groups() { return _file_groups; }

    /**
     * @brief Returns the severity of this report
     *
     * @return Report severity
     */
    [[nodiscard]] Severity severity() const { return _severity; }

    /**
     * @brief Returns the primary diagnostic message
     *
     * @return Message string
     */
    [[nodiscard]] const std::string& message() const { return _message; }

    /**
     * @brief Returns an optional note with additional context
     *
     * @return Optional note string
     */
    [[nodiscard]] const std::optional<std::string>& note() const { return _note; }

    /**
     * @brief Returns optional help text with suggestions
     *
     * @return Optional help string
     */
    [[nodiscard]] const std::optional<std::string>& help() const { return _help; }

    /**
     * @brief Returns an optional error code or identifier
     *
     * @return Optional code string
     */
    [[nodiscard]] const std::optional<std::string>& code() const { return _code; }

private:
    std::optional<std::string> _code, _note, _help;
    MappedFileGroups _file_groups;
    std::string _message;
    Severity _severity;
};

/**
 * @brief Interface implemented by renderers that turn reports into output (e.g., text)
 */
class IReporterRenderer {
public:
    virtual ~IReporterRenderer() = default;

    /**
     * @brief Renders just the severity label (e.g. "error", "warning")
     *
     * @param severity Severity to render
     * @param stream Output stream to write to
     */
    virtual void render(const Severity& severity, std::ostream& stream) = 0;

    /**
     * @brief Renders an entire report
     *
     * @param report Report to be rendered
     * @param stream Output stream to write to
     */
    virtual void render(const Report& report, std::ostream& stream) = 0;

    /**
     * @brief Renders a single file group
     *
     * @param file_group File group to render
     * @param stream Output stream to write to
     */
    virtual void render(const FileGroup& file_group, std::ostream& stream) = 0;

    /**
     * @brief Renders a single line group
     *
     * @param line_group Line group to render
     * @param stream Output stream to write to
     */
    virtual void render(const LineGroup& line_group, std::ostream& stream) = 0;
};

/**
 * @brief Fluent builder for constructing `Report` instances
 */
class Report::Builder {
public:
    /**
     * @brief Sets report severity
     *
     * @param severity Severity to set
     *
     * @return Reference to this builder
     */
    Builder& severity(Severity severity);

    /**
     * @brief Sets the main diagnostic message
     *
     * @param message Diagnostic message text
     *
     * @return Reference to this builder
     */
    Builder& message(std::string message);

    /**
     * @brief Sets an optional error code or identifier
     *
     * @param code Error code or identifier
     *
     * @return Reference to this builder
     */
    Builder& code(std::string code);

    /**
     * @brief Adds a label to the report
     *
     * @param text Label message
     * @param span Source span the label refers to
     *
     * @return Reference to this builder
     */
    Builder& label(std::string text, Span span);

    /**
     * @brief Sets an optional note
     *
     * @param note Note text
     *
     * @return Reference to this builder
     */
    Builder& note(std::string note);

    /**
     * @brief Sets optional help text
     *
     * @param help Help text
     *
     * @return Reference to this builder
     */
    Builder& help(std::string help);

    /**
     * @brief Builds a complete `Report`
     *
     * @return The constructed report
     * @throws std::exception If required fields are missing
     */
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
