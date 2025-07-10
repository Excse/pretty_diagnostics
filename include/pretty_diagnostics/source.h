#pragma once

#include <filesystem>

namespace pretty_diagnostics {

class Location {
public:
    Location(size_t row, size_t column, size_t index);

    friend bool operator==(const Location &lhs, const Location &rhs) {
        return lhs._row == rhs._row
               && lhs._column == rhs._column
               && lhs._index == rhs._index;
    }

    friend bool operator!=(const Location &lhs, const Location &rhs) {
        return !(lhs == rhs);
    }

    [[nodiscard]] auto column() const { return _column; }

    [[nodiscard]] auto index() const { return _index; }

    [[nodiscard]] auto row() const { return _row; }

private:
    size_t _row, _column;
    size_t _index;
};

class Source {
public:
    virtual ~Source() = default;

    [[nodiscard]] virtual Location from_coords(size_t row, size_t column) const = 0;

    [[nodiscard]] virtual Location from_index(size_t index) const = 0;

    [[nodiscard]] virtual std::string substr(const Location &start, const Location &end) const = 0;

    [[nodiscard]] virtual std::string line(const Location &location) const = 0;

    [[nodiscard]] virtual std::string line(size_t line_number) const = 0;

    [[nodiscard]] virtual std::string contents() const = 0;

    [[nodiscard]] virtual std::string path() const = 0;

    [[nodiscard]] virtual size_t size() const = 0;
};

class FileSource final : public Source {
public:
    explicit FileSource(std::filesystem::path path);

    [[nodiscard]] Location from_coords(size_t row, size_t column) const override;

    [[nodiscard]] Location from_index(size_t index) const override;

    [[nodiscard]] std::string substr(const Location &start, const Location &end) const override;

    [[nodiscard]] std::string line(const Location &location) const override;

    [[nodiscard]] std::string line(size_t line_number) const override;

    [[nodiscard]] std::string contents() const override;

    [[nodiscard]] std::string path() const override;

    [[nodiscard]] size_t size() const override;

    friend bool operator==(const FileSource &lhs, const FileSource &rhs) {
        return lhs._path == rhs._path;
    }

    friend bool operator!=(const FileSource &lhs, const FileSource &rhs) {
        return !(lhs == rhs);
    }

private:
    std::filesystem::path _path;
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