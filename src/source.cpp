#include "pretty_diagnostics/source.h"

#include <fstream>

using namespace pretty_diagnostics;

FileSource::FileSource(std::filesystem::path path)
    : _path(std::move(path)) {
    if (!std::filesystem::exists(_path))
        throw std::runtime_error("FileSource::FileSource(): file does not exist: " + _path.string());
}

std::string FileSource::substr(const size_t start, const size_t end) const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("File::substr(): could not open file: " + _path.string());

    stream.seekg(start);
    std::string result(end - start, '\0');
    stream.read(&result[0], end - start);
    result.resize(stream.gcount());

    return result;
}

size_t FileSource::line_number(const size_t start) const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("File::line_number(): could not open file: " + _path.string());

    static constexpr size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    size_t read = 0;
    size_t line = 0;

    // Returns 0-based line number corresponding to byte offset `start`
    while (read < start && stream) {
        const size_t to_read = std::min(BUFFER_SIZE, start - read);
        stream.read(buffer, to_read);
        const size_t actual_read = stream.gcount();

        line += std::count(buffer, buffer + actual_read, '\n');
        read += actual_read;
    }

    return line;
}

std::string FileSource::line(const size_t line) const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("File::line(): could not open file: " + _path.string());

    std::string result;
    for (size_t index = 0; index < line && std::getline(stream, result); ++index) {}

    return result;
}

size_t FileSource::size() const {
    std::ifstream stream(_path, std::ios::binary | std::ios::ate);
    if (!stream.is_open()) throw std::runtime_error("File::size(): could not open file: " + _path.string());

    return stream.tellg();
}

std::string FileSource::contents() const {
    std::ifstream stream(_path, std::ios::binary);
    if (!stream.is_open()) throw std::runtime_error("File::contents(): could not open file: " + _path.string());

    std::ostringstream result;
    result << stream.rdbuf();

    return result.str();
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