#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

#include "pretty_diagnostics/source.hpp"

#include "../../snapshot/snapshot.hpp"

using namespace pretty_diagnostics;

static const auto SNAPSHOTS_DIRECTORY = std::filesystem::path(TEST_PATH) / "pretty_diagnostics" / "source" / "snapshots";

TEST(Source, FileSourceWorking) {
    const auto snapshot_path = SNAPSHOTS_DIRECTORY / "01-source.snapshot";
    const auto file_path = SNAPSHOTS_DIRECTORY / "01-source.c";

    const auto file_name = file_path.filename().stem().string();
    const auto file_source = std::make_shared<FileSource>(file_path);
    file_source->set_working_path(TEST_PATH);

    EXPECT_SNAPSHOT_EQ(file_name, snapshot_path, file_source->contents());

    ASSERT_EQ(file_source->path(), std::filesystem::relative(file_path, TEST_PATH));
    ASSERT_EQ(file_source->line_count(), 6);
    ASSERT_EQ(file_source->size(), 78);
    ASSERT_EQ(file_source->line(4), "    printf(\"Hello World!\\n\");");
}

TEST(Source, FileSourceFailing) {
    const auto file_path = SNAPSHOTS_DIRECTORY / "00-none.c";
    EXPECT_THROW((FileSource(file_path)), std::runtime_error);
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
