#include "pretty_diagnostics/span.h"

using namespace pretty_diagnostics;

Span::Span(const std::shared_ptr<File> &source, const size_t start, const size_t end)
    : _file(source), _start(start), _end(end) {
    if (start >= end) {
        throw std::runtime_error("Span::Span(): start must be less than end");
    }

    const auto size = _file->contents().size();
    if (start > size) {
        throw std::runtime_error("Span::Span(): start must be less than or equal to file size");
    }

    if (end > size) {
        throw std::runtime_error("Span::Span(): end must be less than or equal to file size");
    }
}

std::string Span::contents() const {
    const auto &contents = _file->contents();
    return contents.substr(_start, this->width());
}

size_t Span::width() const {
    return _end - _start;
}

size_t Span::line() const {
    const auto &contents = _file->contents();
    const auto begin = contents.begin();
    const auto end = contents.begin() + static_cast<long>(_start);
    return std::count(begin, end, '\n');
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
