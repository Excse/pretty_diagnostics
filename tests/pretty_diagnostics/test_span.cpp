#include "gtest/gtest.h"

#include "pretty_diagnostics/span.hpp"

using namespace pretty_diagnostics;

static const auto RESOURCES_DIRECTORY = std::filesystem::path(TEST_PATH) / "pretty_diagnostics" / "resources";

TEST(Span, FirstLabel) {
    const auto file_path = RESOURCES_DIRECTORY / "01-main.c";

    const auto file_source = std::make_shared<FileSource>(file_path, TEST_PATH);

    const auto span = Span(file_source, 0, 18);
    ASSERT_EQ(span.source(), file_source);
    ASSERT_EQ(span.start(), Location(0, 0, 0));
    ASSERT_EQ(span.end(), Location(0, 18, 18));
    ASSERT_EQ(span.width(), 18);
    ASSERT_EQ(span.line(), 0);
    ASSERT_EQ(span.substr(), "#include <stdio.h>");
}

TEST(Span, SecondLabel) {
    const auto file_path = RESOURCES_DIRECTORY / "01-main.c";

    const auto file_source = std::make_shared<FileSource>(file_path, TEST_PATH);

    const auto span = Span(file_source, 37, 43);
    ASSERT_EQ(span.source(), file_source);
    ASSERT_EQ(span.start(), Location(3, 4, 37));
    ASSERT_EQ(span.end(), Location(3, 10, 43));
    ASSERT_EQ(span.width(), 6);
    ASSERT_EQ(span.line(), 3);
    ASSERT_EQ(span.substr(), "printf");
}

TEST(Span, ThirdLabel) {
    const auto file_path = RESOURCES_DIRECTORY / "01-main.c";

    const auto file_source = std::make_shared<FileSource>(file_path, TEST_PATH);

    const auto span = Span(file_source, 44, 60);
    ASSERT_EQ(span.source(), file_source);
    ASSERT_EQ(span.start(), Location(3, 11, 44));
    ASSERT_EQ(span.end(), Location(3, 27, 60));
    ASSERT_EQ(span.width(), 16);
    ASSERT_EQ(span.line(), 3);
    ASSERT_EQ(span.substr(), "\"Hello World!\\n\"");
}

TEST(Span, InvalidRange) {
    const auto file_path = RESOURCES_DIRECTORY / "01-main.c";

    const auto file_source = std::make_shared<FileSource>(file_path, TEST_PATH);

    ASSERT_THROW(Span(file_source, 16, 0), std::runtime_error);
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
