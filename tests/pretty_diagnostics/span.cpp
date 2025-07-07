#include "gtest/gtest.h"

#include "pretty_diagnostics/span.h"

using namespace pretty_diagnostics;

TEST(Span, ValidExample) {
    const auto file = std::make_shared<FileSource>("resources/example");
    const auto span = Span(file, 0, 16);

    ASSERT_EQ(span.source(), file);
    ASSERT_EQ(span.start(), 0);
    ASSERT_EQ(span.end(), 16);
    ASSERT_EQ(span.width(), 16);
    ASSERT_EQ(span.line(), 0);
    ASSERT_EQ(span.contents(), "Thes ist a test!");
}

TEST(Span, SecondLine) {
    const auto file = std::make_shared<FileSource>("resources/example");
    const auto span = Span(file, 17, 38);

    ASSERT_EQ(span.source(), file);
    ASSERT_EQ(span.start(), 17);
    ASSERT_EQ(span.end(), 38);
    ASSERT_EQ(span.width(), 21);
    ASSERT_EQ(span.line(), 1);
    ASSERT_EQ(span.contents(), "This is another line.");
}

TEST(Span, InvalidRange) {
    const auto file = std::make_shared<FileSource>("resources/example");

    ASSERT_THROW(Span(file, 16, 0), std::runtime_error);
    ASSERT_THROW(Span(file, 16, 16), std::runtime_error);
    ASSERT_THROW(Span(file, 0, 53), std::runtime_error);
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
