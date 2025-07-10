#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

#include "pretty_diagnostics/renderer.h"
#include "pretty_diagnostics/report.h"
#include "pretty_diagnostics/source.h"

#include "../utils/snapshot.h"

using namespace pretty_diagnostics;

TEST(Report, BuilderCorrect) {
    const auto file = std::make_shared<FileSource>("resources/main.c");

    constexpr auto severity = Severity::Error;
    constexpr auto message = "Displaying a brief summary of what happened";
    constexpr auto code = "E1337";

    const auto report = Report::Builder()
            .severity(severity)
            .message(message)
            .code(code)
            .label("And this is the function that actually makes the magic happen", {file, 37, 43})
            .label("This is the string that is getting printed to the console", {file, 44, 60})
            .label("Relevant include to enable the usage of printf", {file, 10, 17})
            .build();

    ASSERT_EQ(report.severity(), severity);
    ASSERT_EQ(report.message(), message);
    ASSERT_EQ(report.code(), code);

    ASSERT_EQ(report.file_groups().size(), 1);
    const auto &file_group = report.file_groups().at(file);
    ASSERT_EQ(file_group.source(), file);

    ASSERT_EQ(file_group.line_groups().size(), 2);
    const auto &line_1_group = file_group.line_groups().at(1);
    ASSERT_EQ(line_1_group.labels().size(), 1);
    const auto &line_4_group = file_group.line_groups().at(4);
    ASSERT_EQ(line_4_group.labels().size(), 2);
}

TEST(Report, CorrectTextRender) {
    const auto file = std::make_shared<FileSource>("resources/main.c");

    const auto report = Report::Builder()
            .severity(Severity::Error)
            .message("Displaying a brief summary of what happened")
            .code("E1337")
            .label("And this is the function that actually makes the magic happen", {file, 37, 43})
            .label("This is the string that is getting printed to the console", {file, 44, 60})
            .label("Relevant include to enable the usage of printf", {file, 10, 17})
            .build();

    auto renderer = TextRenderer(report);
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
