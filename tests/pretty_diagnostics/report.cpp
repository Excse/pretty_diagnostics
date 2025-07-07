#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

#include "pretty_diagnostics/renderer.h"
#include "pretty_diagnostics/report.h"
#include "pretty_diagnostics/file.h"

#include "../utils/snapshot.h"

using namespace pretty_diagnostics;

TEST(Report, BuilderCorrect) {
    const auto file = std::make_shared<File>(TEST_PATH "/resources/example");

    constexpr auto severity = Severity::Error;
    constexpr auto message = "Displaying a brief summary of what happened";
    constexpr auto code = "E1337";
    constexpr auto label_text = "Giving some tips or extra details about what is wrong here";
    const auto label_span = Span{file, 5, 8};

    const auto report = Report::Builder()
            .severity(severity)
            .message(message)
            .code(code)
            .label(label_text, label_span)
            .build();

    ASSERT_EQ(report.severity(), Severity::Error);
    ASSERT_EQ(report.message(), message);
    ASSERT_EQ(report.code(), code);
    ASSERT_EQ(report.labels().size(), 1);
    const auto &label = report.labels().front();
    ASSERT_EQ(label.text(), label_text);
    ASSERT_EQ(label.span(), label_span);
    ASSERT_EQ(label.span().contents(), "ist");
}

TEST(Report, CorrectTextRender) {
    const auto file = std::make_shared<File>(TEST_PATH "/resources/example");

    constexpr auto severity = Severity::Error;
    constexpr auto message = "Displaying a brief summary of what happened";
    constexpr auto code = "E1337";
    constexpr auto label_text = "Giving some tips or extra details about what is wrong here";
    const auto label_span = Span{file, 5, 8};

    const auto report = Report::Builder()
            .severity(severity)
            .message(message)
            .code(code)
            .label(label_text, label_span)
            .build();

    const auto renderer = TextRenderer();
    auto stream = std::ostringstream();
    report.render(renderer, stream);

    EXPECT_SNAPSHOT_EQ(TextRender, stream.str());
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
