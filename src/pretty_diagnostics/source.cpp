#include "pretty_diagnostics/source.hpp"
#include "pretty_diagnostics/utils.hpp"

#include <algorithm>
#include <fstream>

using namespace pretty_diagnostics;

Location::Location(const size_t row, const size_t column, const size_t index) :
    _row(row), _column(column), _index(index) {
}

StringSource::StringSource(std::string contents, std::string display_path) :
    _display_path(std::move(display_path)), _contents(std::move(contents)) {
    _line_starts.push_back(0);
    for (size_t index = 0; index < _contents.size(); ++index) {
        if (_contents[index] == '\n') {
            _line_starts.push_back(index + 1);
        }
    }
}

Location StringSource::from_coords(size_t row, size_t column) const {
    if (row >= _line_starts.size()) {
        throw std::runtime_error("StringSource::from_coords(): invalid coordinates, there are not enough rows present");
    }

    const auto line_start = _line_starts[row];
    const auto line_text = this->line(row);
    const auto byte_column = from_visual_column(line_text, column);

    return { row, column, line_start + byte_column };
}

Location StringSource::from_index(const size_t index) const {
    if (index > _contents.size()) {
        throw std::runtime_error("StringSource::from_index(): invalid index, out of bounds");
    }

    const auto it = std::ranges::upper_bound(_line_starts, index);
    const auto row = (it == _line_starts.begin()) ? 0 : static_cast<size_t>(std::distance(_line_starts.begin(), it) - 1);
    const auto byte_column = index - _line_starts[row];

    const auto line_text = this->line(row);
    const auto visual_column = to_visual_column(line_text, byte_column);

    return { row, visual_column, index };
}

std::string StringSource::substr(const Location& start, const Location& end) const {
    const auto start_index = start.index();
    const auto end_index = end.index();

    if (end_index < start_index || end_index > _contents.size() || start_index > _contents.size()) {
        throw std::runtime_error("StringSource::substr(): invalid range");
    }

    return _contents.substr(start_index, end_index - start_index);
}

std::string StringSource::line(const Location& location) const {
    return StringSource::line(location.row());
}

std::string StringSource::line(const size_t line_number) const {
    if (line_number >= _line_starts.size()) {
        throw std::runtime_error("StringSource::line(): invalid line number, there are not enough lines present");
    }

    const size_t row = line_number;
    const auto line_start = _line_starts[row];
    const auto line_end = (row + 1 < _line_starts.size()) ? _line_starts[row + 1] : _contents.size();

    auto result = _contents.substr(line_start, line_end - line_start);
    if (!result.empty() && result.back() == '\n') result.pop_back();
    if (!result.empty() && result.back() == '\r') result.pop_back();

    return result;
}

size_t StringSource::line_count() const {
    return _line_starts.size();
}

const std::string& StringSource::contents() const {
    return _contents;
}

std::string StringSource::path() const {
    return _display_path;
}

size_t StringSource::size() const {
    return _contents.size();
}

FileSource::FileSource(const std::filesystem::path& path, const std::filesystem::path& working_path)
    : StringSource(_read_contents(path), std::filesystem::relative(path, working_path).string()) {}

std::string FileSource::_read_contents(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("FileSource::_read_contents(): file does not exist: " + path.string());
    }

    std::ifstream stream(path, std::ios::binary | std::ios::ate);
    if (!stream.is_open()) {
        throw std::runtime_error("FileSource::_read_contents(): could not open file: " + path.string());
    }

    const auto size = stream.tellg();
    if (size < 0) {
        throw std::runtime_error("FileSource::_read_contents(): failed to determine file size: " + path.string());
    }

    stream.seekg(0);

    std::string contents;
    contents.resize(size);

    if (!stream.read(contents.data(), size)) {
        throw std::runtime_error("FileSource::_read_contents(): failed to read file: " + path.string());
    }

    return contents;
}

std::ostream& operator<<(std::ostream& os, const Location& location) {
    os << "Location(";
    os << "row=\"" << location.row() << "\", ";
    os << "column=\"" << location.column() << "\", ";
    os << "index=\"" << location.index() << "\"";
    os << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const StringSource& source) {
    os << "StringSource(";
    os << "path=\"" << source.path() << "\", ";
    os << "size=\"" << source.size() << "\"";
    os << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const FileSource& source) {
    os << "FileSource(";
    os << "path=\"" << source.path() << "\", ";
    os << "size=\"" << source.size() << "\"";
    os << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Source& source) {
    os << "Source(";
    os << "path=\"" << source.path() << "\", ";
    os << "size=\"" << source.size() << "\"";
    os << ")";
    return os;
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
