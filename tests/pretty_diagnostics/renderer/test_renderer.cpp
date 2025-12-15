#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

#include "pretty_diagnostics/renderer.hpp"

#include "../../snapshot/snapshot.hpp"

using namespace pretty_diagnostics;

static const auto SNAPSHOTS_DIRECTORY = std::filesystem::path(TEST_PATH) / "pretty_diagnostics" / "renderer" / "snapshots";

TEST(Renderer, CorrectTextWrap) {
    const auto result = TextRenderer::wrap_text("Hello World!\nHow are you, today?", 10);
    ASSERT_EQ(result, std::vector<std::string>({ "Hello", "World!", "How are", "you,", "today?" }));
}

TEST(Renderer, SingleCharWrap) {
    const auto result = TextRenderer::wrap_text("Hello!", 1);
    ASSERT_EQ(result, std::vector<std::string>({ "H", "e", "l", "l", "o", "!" }));
}

TEST(Renderer, LongHardSplit) {
    const auto result = TextRenderer::wrap_text("AAAAAAAAAA BBBBBBBBBB", 5);
    ASSERT_EQ(result, std::vector<std::string>({ "AAAAA", "AAAAA", "BBBBB", "BBBBB" }));
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
    const auto snapshot_path = SNAPSHOTS_DIRECTORY / "01-renderer.snapshot";
    const auto file_path = SNAPSHOTS_DIRECTORY / "01-renderer.c";

    const auto file_name = file_path.filename().stem().string();
    const auto file_source = std::make_shared<FileSource>(file_path);
    file_source->set_working_path(TEST_PATH);

    const auto report = Report::Builder()
                       .severity(Severity::Error)
                       .message("Displaying a brief summary of what happened")
                       .code("E1337")
                       .label("And this is the function that actually makes the magic happen", { file_source, 37, 43 })
                       .label("This is the string that is getting printed to the console", { file_source, 44, 60 })
                       .label("Relevant include to enable the usage of printf", { file_source, 10, 17 })
                       .label("This is a new line", { file_source, 1, 0, 1, 1 })
                       .note("This example showcases every little detail of the library, also with the capability of line wrapping.")
                       .help("Visit https://github.com/Excse/pretty_diagnostics for more help.")
                       .build();

    auto renderer = TextRenderer(report);
    auto stream = std::ostringstream();
    report.render(renderer, stream);

    EXPECT_SNAPSHOT_EQ(file_name, snapshot_path, stream.str());
}

TEST(Renderer, HardTextRender) {
    const auto snapshot_path = SNAPSHOTS_DIRECTORY / "02-renderer.snapshot";
    const auto file_path = SNAPSHOTS_DIRECTORY / "02-renderer.c";

    const auto file_name = file_path.filename().stem().string();
    const auto file_source = std::make_shared<FileSource>(file_path);
    file_source->set_working_path(TEST_PATH);

    static constexpr auto LOREM = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse semper hendrerit iaculis. Integer suscipit "
            "facilisis libero sed consectetur. Fusce turpis risus, elementum nec fermentum quis, ultricies a libero. Aliquam "
            "et nisi quis elit pulvinar vestibulum sit amet id est. Integer.";

    const auto report = Report::Builder()
                       .severity(Severity::Error)
                       .message("Displaying a brief summary of what happened")
                       .code("E1337")
                       .label(LOREM, { file_source, 37, 40 })
                       .label(LOREM, { file_source, 40, 41 })
                       .label(LOREM, { file_source, 41, 43 })
                       .label(LOREM, { file_source, 44, 51 })
                       .label(LOREM, { file_source, 51, 52 })
                       .label(LOREM, { file_source, 52, 60 })
                       .label(LOREM, { file_source, 10, 13 })
                       .label(LOREM, { file_source, 13, 14 })
                       .label(LOREM, { file_source, 14, 17 })
                       .note(LOREM)
                       .help(LOREM)
                       .build();

    auto renderer = TextRenderer(report);
    auto stream = std::ostringstream();
    report.render(renderer, stream);

    EXPECT_SNAPSHOT_EQ(file_name, snapshot_path, stream.str());
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
