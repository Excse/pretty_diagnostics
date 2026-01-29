#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace pretty_diagnostics {
/**
 * @brief A position inside a `Source`, expressed as (row, column, index)
 *
 * The `index` is a 0-based absolute character offset into the source contents.
 * `row` and `column` represent human-readable coordinates, the `index` defines ordering
 */
class Location {
public:
    /**
     * @brief Constructs a location with the given coordinates and absolute index
     *
     * @param row 0-based line number
     * @param column 0-based column number
     * @param index 0-based absolute character index
     */
    Location(size_t row, size_t column, size_t index);

    /**
     * @brief Constructs a default-initialized location (all -1).
     */
    Location() = delete;

    /**
     * @brief Orders locations by their absolute index
     *
     * @param lhs Left-hand location
     * @param rhs Right-hand location
     *
     * @return True if @p lhs precedes @p rhs by absolute index
     */
    friend bool operator<(const Location& lhs, const Location& rhs) {
        return lhs._index < rhs._index;
    }

    /**
     * @brief Less-than-or-equal comparison derived from `>=`
     *
     * @param lhs Left-hand location
     * @param rhs Right-hand location
     *
     * @return True if @p lhs is not greater than @p rhs
     */
    friend bool operator<=(const Location& lhs, const Location& rhs) {
        return rhs >= lhs;
    }

    /**
     * @brief Greater-than comparison derived from `<`
     *
     * @param lhs Left-hand location
     * @param rhs Right-hand location
     *
     * @return True if @p lhs succeeds @p rhs by absolute index
     */
    friend bool operator>(const Location& lhs, const Location& rhs) {
        return rhs < lhs;
    }

    /**
     * @brief Greater-than-or-equal comparison derived from `<`
     *
     * @param lhs Left-hand location
     * @param rhs Right-hand location
     *
     * @return True if @p lhs is not less than @p rhs
     */
    friend bool operator>=(const Location& lhs, const Location& rhs) {
        return !(lhs < rhs);
    }

    /**
     * @brief Equality compares row, column and index
     *
     * @param lhs Left-hand location
     * @param rhs Right-hand location
     *
     * @return True if all coordinates and index are equal
     */
    friend bool operator==(const Location& lhs, const Location& rhs) {
        return lhs._row == rhs._row
               && lhs._column == rhs._column
               && lhs._index == rhs._index;
    }

    /**
     * @brief Inequality based on `operator==`
     *
     * @param lhs Left-hand location
     * @param rhs Right-hand location
     *
     * @return True if the locations differ
     */
    friend bool operator!=(const Location& lhs, const Location& rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Returns the 0-based column number
     *
     * @return 0-based column number
     */
    [[nodiscard]] auto column() const { return _column; }

    /**
     * @brief Returns the 0-based absolute character index
     *
     * @return 0-based absolute index
     */
    [[nodiscard]] auto index() const { return _index; }

    /**
     * @brief Returns the 0-based row (line) number
     *
     * @return 0-based row number
     */
    [[nodiscard]] auto row() const { return _row; }

private:
    size_t _row, _column;
    size_t _index;
};

/**
 * @brief Abstract interface for reading and mapping source text
 */
class Source {
public:
    virtual ~Source() = default;

    /**
     * @brief Returns a location corresponding to the given row and column
     *
     * @param row 0-based line number
     * @param column 0-based column number
     *
     * @return Mapped location.
     */
    [[nodiscard]] virtual Location from_coords(size_t row, size_t column) const = 0;

    /**
     * @brief Returns a location for the given absolute character index
     *
     * @param index 0-based absolute character index
     *
     * @return Mapped location
     */
    [[nodiscard]] virtual Location from_index(size_t index) const = 0;

    /**
     * @brief Returns the substring between two locations
     *
     * @param start Inclusive start location
     * @param end Exclusive end location
     *
     * @return Substring between @p start and @p end
     */
    [[nodiscard]] virtual std::string substr(const Location& start, const Location& end) const = 0;

    /**
     * @brief Returns the full line at the given location
     *
     * @param location A location within the desired line
     *
     * @return The entire line contents without a trailing newline
     */
    [[nodiscard]] virtual std::string line(const Location& location) const = 0;

    /**
     * @brief Returns the contents of the specified line number
     *
     * @param line_number 0-based line number
     *
     * @return The entire line contents without a trailing newline
     */
    [[nodiscard]] virtual std::string line(size_t line_number) const = 0;

    /**
     * @brief Returns the total number of lines in the source
     *
     * @return Line count
     */
    [[nodiscard]] virtual size_t line_count() const = 0;

    /**
     * @brief Returns the entire contents of the source
     *
     * @return Full source contents
     */
    [[nodiscard]] virtual const std::string& contents() const = 0;

    /**
     * @brief Returns a displayable path or identifier of the source
     *
     * @return Display path or identifier
     */
    [[nodiscard]] virtual std::string path() const = 0;

    /**
     * @brief Returns the total size (in characters) of the source
     *
     * @return Size of the source in characters
     */
    [[nodiscard]] virtual size_t size() const = 0;
};

/**
 * @brief A `Source` implementation that reads from an in-memory string
 */
class StringSource : public Source {
public:
    /**
     * @brief Creates a string source with an optional display path
     *
     * @param contents Source contents held in memory
     * @param display_path Optional display identifier for diagnostics output
     */
    explicit StringSource(std::string contents, std::string display_path = "<memory>");

    /**
     * @brief Maps (row, column) to a `Location` within the string
     *
     * @param row 0-based line number
     * @param column 0-based column number
     *
     * @return Location corresponding to the given coordinates
     */
    [[nodiscard]] Location from_coords(size_t row, size_t column) const override;

    /**
     * @brief Maps an absolute index to a `Location` within the string
     *
     * @param index 0-based absolute character index
     *
     * @return Location corresponding to the given index
     */
    [[nodiscard]] Location from_index(size_t index) const override;

    /**
     * @brief Extracts a substring delimited by two locations
     *
     * @param start Inclusive start location
     * @param end Exclusive end location
     *
     * @return Substring between @p start and @p end
     */
    [[nodiscard]] std::string substr(const Location& start, const Location& end) const override;

    /**
     * @brief Returns the contents of the line containing the given location
     *
     * @param location A location within the desired line
     *
     * @return The entire line contents without a trailing newline
     */
    [[nodiscard]] std::string line(const Location& location) const override;

    /**
     * @brief Returns the contents of the specified line number
     *
     * @param line_number 0-based line number
     *
     * @return The entire line contents without a trailing newline
     */
    [[nodiscard]] std::string line(size_t line_number) const override;

    /**
     * @brief Returns the total number of lines in the string
     *
     * @return Line count
     */
    [[nodiscard]] size_t line_count() const override;

    /**
     * @brief Returns the entire contents of the source
     *
     * @return Full source contents
     */
    [[nodiscard]] const std::string& contents() const override;

    /**
     * @brief Returns a displayable path or identifier of the source
     *
     * @return Display path or identifier
     */
    [[nodiscard]] std::string path() const override;

    /**
     * @brief Returns the total size (in characters) of the source
     *
     * @return Size in characters
     */
    [[nodiscard]] size_t size() const override;

private:
    std::vector<size_t> _line_starts;
    std::string _display_path;
    std::string _contents;
};

/**
 * @brief A `Source` implementation that reads from a file on disk
 */
class FileSource final : public StringSource {
public:
    /**
     * @brief Creates a file source from a filesystem path
     *
     * @param path Path to the file on disk (absolute or relative)
     * @param working_path Optional path to make the path relative
     */
    explicit FileSource(const std::filesystem::path& path, const std::filesystem::path& working_path = std::filesystem::current_path());

    /**
     * @brief Equality compares path
     *
     * @param lhs Left-hand file source
     * @param rhs Right-hand file source
     *
     * @return True if paths are equal
     */
    friend bool operator==(const FileSource& lhs, const FileSource& rhs) {
        return lhs.path() == rhs.path();
    }

    /**
     * @brief Inequality based on `operator==`
    *
     * @param lhs Left-hand file source
     * @param rhs Right-hand file source
     *
     * @return True if the paths differ
     */
    friend bool operator!=(const FileSource& lhs, const FileSource& rhs) {
        return !(lhs == rhs);
    }

private:
    [[nodiscard]] static std::string _read_contents(const std::filesystem::path& path);
};
} // namespace pretty_diagnostics

/**
 * @brief Streams a readable description of a `Location`
 *
 * @param os Output stream to write to
 * @param location Location to format
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const pretty_diagnostics::Location& location);

/**
 * @brief Streams a readable description of a `StringSource`
 *
 * @param os Output stream to write to
 * @param source Source to describe
 *
 * @return Reference to @p os.
 */
std::ostream& operator<<(std::ostream& os, const pretty_diagnostics::StringSource& source);

/**
 * @brief Streams a readable description of a `FileSource`
 *
 * @param os Output stream to write to
 * @param source Source to describe
 *
 * @return Reference to @p os.
 */
std::ostream& operator<<(std::ostream& os, const pretty_diagnostics::FileSource& source);

/**
 * @brief Streams a readable description of a `Source`
 *
 * @param os Output stream to write to
 * @param source Source to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const pretty_diagnostics::Source& source);

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
