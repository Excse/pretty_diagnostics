#include "gtest/gtest.h"

#include "pretty_diagnostics/span.h"

using namespace pretty_diagnostics;

TEST(Span, FirstLabel) {
    const auto file = std::make_shared<FileSource>("resources/main.c");
    const auto span = Span(file, 0, 18);

    ASSERT_EQ(span.source(), file);
    ASSERT_EQ(span.start(), Location(0, 0, 0));
    ASSERT_EQ(span.end(), Location(0, 18, 18));
    ASSERT_EQ(span.width(), 18);
    ASSERT_EQ(span.line(), 1);
    ASSERT_EQ(span.substr(), "#include <stdio.h>");
}

TEST(Span, SecondLabel) {
    const auto file = std::make_shared<FileSource>("resources/main.c");
    const auto span = Span(file, 37, 43);

    ASSERT_EQ(span.source(), file);
    ASSERT_EQ(span.start(), Location(3, 4, 37));
    ASSERT_EQ(span.end(), Location(3, 10, 43));
    ASSERT_EQ(span.width(), 6);
    ASSERT_EQ(span.line(), 4);
    ASSERT_EQ(span.substr(), "printf");
}

TEST(Span, ThirdLabel) {
    const auto file = std::make_shared<FileSource>("resources/main.c");
    const auto span = Span(file, 44, 60);

    ASSERT_EQ(span.source(), file);
    ASSERT_EQ(span.start(), Location(3, 11, 44));
    ASSERT_EQ(span.end(), Location(3, 27, 60));
    ASSERT_EQ(span.width(), 16);
    ASSERT_EQ(span.line(), 4);
    ASSERT_EQ(span.substr(), "\"Hello World!\\n\"");
}

TEST(Span, InvalidRange) {
    const auto file = std::make_shared<FileSource>("resources/main.c");

    ASSERT_THROW(Span(file, 16, 0), std::runtime_error);
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
