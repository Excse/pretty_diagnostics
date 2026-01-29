#include "pretty_diagnostics/span.hpp"

#include "pretty_diagnostics/utils.hpp"

using namespace pretty_diagnostics;

Span::Span(const std::shared_ptr<Source>& source, const Location& start, const Location& end) :
    _source(source), _start(start), _end(end) {
    if (start.index() > end.index()) {
        throw std::runtime_error("Span::Span(): start location must be smaller than the end location");
    }
}

Span::Span(const std::shared_ptr<Source>& source,
           const size_t start_row, const size_t start_column,
           const size_t end_row, const size_t end_column) :
    Span(source, source->from_coords(start_row, start_column), source->from_coords(end_row, end_column)) {
}

Span::Span(const std::shared_ptr<Source>& source,
           const size_t start_index,
           const size_t end_index) :
    Span(source, source->from_index(start_index), source->from_index(end_index)) {
}

Span Span::join(const Span& other) const {
    return { _source, std::min(_start, other._start), std::max(_end, other._end) };
}

bool Span::intersects(const Span& other) const {
    return _start.index() <= other._end.index() && _end.index() > other._start.index();
}

std::string Span::substr() const {
    return _source->substr(_start, _end);
}

size_t Span::width() const {
    return _end.index() - _start.index();
}

size_t Span::line() const {
    return _start.row();
}

std::ostream& operator<<(std::ostream& os, const Span& span) {
    os << "Span(";
    os << "contents=\"" << escape_string(span.substr()) << "\", ";
    os << "start=\"" << span.start() << "\", ";
    os << "end=\"" << span.end() << "\", ";
    os << "source=\"" << *span.source() << "\"";
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
