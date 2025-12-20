#include "pretty_diagnostics/source.hpp"

#include <algorithm>
#include <fstream>

using namespace pretty_diagnostics;

Location::Location(const size_t row, const size_t column, const size_t index) :
    _row(row), _column(column), _index(index) {
}

Location::Location() :
    _row(-1), _column(-1), _index(-1) {
}

FileSource::FileSource(std::filesystem::path path) :
    _path(std::move(path)) {
    if (!std::filesystem::exists(_path)) throw std::runtime_error("FileSource::FileSource(): file does not exist: " + _path.string());
}

void FileSource::set_working_path(const std::filesystem::path& path) {
    _working_path = path;
}

Location FileSource::from_coords(size_t row, size_t column) const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("FileSource::from_coords(): could not open file: " + _path.string());

    size_t current_row = 0;
    size_t index = 0;

    std::string line;
    while (std::getline(stream, line)) {
        // We have to adjust for the newline character, so add one.
        const auto line_length = line.length() + 1;

        if (current_row != row) {
            index += line_length;
            current_row++;
            continue;
        }

        if (column > line_length) throw std::runtime_error("FileSource::from_coords(): invalid coordinates, column is out of line bounds");

        return { row, column, index + column };
    }

    throw std::runtime_error("FileSource::from_coords(): invalid coordinates, there are not enough rows present");
}

Location FileSource::from_index(size_t index) const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("FileSource::from_index(): could not open file: " + _path.string());

    size_t current_index = 0;
    size_t row = 0;

    std::string line;
    while (std::getline(stream, line)) {
        const auto line_length = line.length() + 1;

        if (current_index + line_length < index) {
            current_index += line_length;
            row++;
            continue;
        }

        const auto column = index - current_index;
        if (column > line_length) throw std::runtime_error("FileSource::from_index(): invalid index, column is out of line bounds");

        return { row, index - current_index, index };
    }

    throw std::runtime_error("FileSource::from_index(): invalid index, there are not enough rows present");
}

std::string FileSource::substr(const Location& start, const Location& end) const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("File::substr(): could not open file: " + _path.string());

    const auto start_index = static_cast<long>(start.index());
    const auto width = static_cast<long>(end.index()) - static_cast<long>(start.index());

    stream.seekg(start_index);
    std::string result(width, '\0');
    stream.read(&result[0], width);
    result.resize(stream.gcount());

    return result;
}

std::string FileSource::line(const Location& location) const {
    return FileSource::line(location.row() + 1);
}

std::string FileSource::line(const size_t line_number) const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("File::line(): could not open file: " + _path.string());

    std::string result;

    for (size_t index = 0; index < line_number; ++index) {
        if (std::getline(stream, result)) continue;
        throw std::runtime_error("File::line(): invalid line number, there are not enough lines present");
    }

    return result;
}

size_t FileSource::line_count() const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("FileSource::line_count(): could not open file: " + _path.string());

    size_t count = 0;

    std::string line;
    while (std::getline(stream, line)) count++;

    return count;
}

std::string FileSource::contents() const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("File::contents(): could not open file: " + _path.string());

    std::ostringstream result;
    result << stream.rdbuf();

    return result.str();
}

std::string FileSource::path() const {
    if (!_working_path) return _path.string();
    return std::filesystem::relative(_path, _working_path.value()).string();
}

size_t FileSource::size() const {
    std::ifstream stream(_path, std::ios::binary | std::ios::ate);
    if (!stream.is_open()) throw std::runtime_error("File::size(): could not open file: " + _path.string());

    return stream.tellg();
}

std::ostream& operator<<(std::ostream& os, const Location& location) {
    os << "Location(";
    os << "row=\"" << location.row() << "\", ";
    os << "column=\"" << location.column() << "\", ";
    os << "index=\"" << location.index() << "\"";
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
