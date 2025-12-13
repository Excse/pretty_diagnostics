#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

#include "pretty_diagnostics/renderer.h"

#include "../utils/snapshot.h"

using namespace pretty_diagnostics;

TEST(Renderer, CorrectTextWrap) {
    const auto result = TextRenderer::wrap_text("Hello World!\nHow are you, today?", 10);
    ASSERT_EQ(result, std::vector<std::string>({"Hello", "World!", "How are", "you,", "today?"}));
}

TEST(Renderer, SingleCharWrap) {
    const auto result = TextRenderer::wrap_text("Hello!", 1);
    ASSERT_EQ(result, std::vector<std::string>({"H", "e", "l", "l", "o", "!"}));
}

TEST(Renderer, LongHardSplit) {
    const auto result = TextRenderer::wrap_text("AAAAAAAAAA BBBBBBBBBB", 5);
    ASSERT_EQ(result, std::vector<std::string>({"AAAAA", "AAAAA", "BBBBB", "BBBBB"}));
}

TEST(Renderer, RealExample) {
    const auto result = TextRenderer::wrap_text(
        "This example showcases every little detail of the library, also with the capability of line wrapping.",
        69);
    ASSERT_EQ(result, std::vector<std::string>({
                  "This example showcases every little detail of the library, also with",
                  "the capability of line wrapping."
                  }));
}

TEST(Renderer, SimpleTextRender) {
    const auto file = std::make_shared<FileSource>("resources/main.c");

    const auto report = Report::Builder()
            .severity(Severity::Error)
            .message("Displaying a brief summary of what happened")
            .code("E1337")
            .label("And this is the function that actually makes the magic happen", {file, 37, 43})
            .label("This is the string that is getting printed to the console", {file, 44, 60})
            .label("Relevant include to enable the usage of printf", {file, 10, 17})
            .label("This is a new line", {file, 1, 0, 1, 1})
            .note("This example showcases every little detail of the library, also with the capability of line wrapping.")
            .help("Visit https://github.com/Excse/pretty_diagnostics for more help.")
            .build();

    auto renderer = TextRenderer(report);
    auto stream = std::ostringstream();
    report.render(renderer, stream);

    EXPECT_SNAPSHOT_EQ(SimpleText, stream.str());
}

TEST(Renderer, HardTextRender) {
    const auto file = std::make_shared<FileSource>("resources/main.c");

    static constexpr auto LOREM = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse semper hendrerit iaculis. Integer suscipit facilisis libero sed consectetur. Fusce turpis risus, elementum nec fermentum quis, ultricies a libero. Aliquam et nisi quis elit pulvinar vestibulum sit amet id est. Integer.";

    const auto report = Report::Builder()
            .severity(Severity::Error)
            .message("Displaying a brief summary of what happened")
            .code("E1337")
            .label(LOREM, {file, 37, 40})
            .label(LOREM, {file, 40, 41})
            .label(LOREM, {file, 41, 43})
            .label(LOREM, {file, 44, 51})
            .label(LOREM, {file, 51, 52})
            .label(LOREM, {file, 52, 60})
            .label(LOREM, {file, 10, 13})
            .label(LOREM, {file, 13, 14})
            .label(LOREM, {file, 14, 17})
            .note(LOREM)
            .help(LOREM)
            .build();

    auto renderer = TextRenderer(report);
    auto stream = std::ostringstream();
    report.render(renderer, stream);

    EXPECT_SNAPSHOT_EQ(HardText, stream.str());
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
