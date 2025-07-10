#pragma once

#include "source.h"

namespace pretty_diagnostics {

class Span {
private:
    Span(const std::shared_ptr<Source> &source, const Location &start, const Location &end);

public:
    Span(const std::shared_ptr<Source> &source,
         size_t start_row, size_t start_column,
         size_t end_row, size_t end_column);

    Span(const std::shared_ptr<Source> &source,
         size_t start_index,
         size_t end_index);

    friend bool operator==(const Span &lhs, const Span &rhs) {
        return lhs._source == rhs._source
               && lhs._start == rhs._start
               && lhs._end == rhs._end;
    }

    friend bool operator!=(const Span &lhs, const Span &rhs) {
        return !(lhs == rhs);
    }

    [[nodiscard]] bool intersects(const Span &other) const;

    [[nodiscard]] std::string substr() const;

    [[nodiscard]] size_t width() const;

    [[nodiscard]] size_t line() const;

    [[nodiscard]] auto &source() const { return _source; }

    [[nodiscard]] auto start() const { return _start; }

    [[nodiscard]] auto end() const { return _end; }

private:
    std::shared_ptr<Source> _source;
    Location _start, _end;
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