#include "gtest/gtest.h"

#include "pretty_diagnostics/utils.hpp"

using namespace pretty_diagnostics;

TEST(Utils, StreamWidth) {
    const std::stringstream stream;
    const auto stream_width = get_stream_width(stream);
    EXPECT_EQ(stream_width, std::numeric_limits<size_t>::max());
}

TEST(Utils, StandardOutWidth) {
    if (!isatty(STDOUT_FILENO)) {
        GTEST_SKIP() << "stdout is not a terminal, skipping width test";
    }

    const auto stream_width = get_stream_width(std::cout);
    EXPECT_NE(stream_width, std::numeric_limits<size_t>::max());
}

TEST(Utils, StandardErrorWidth) {
    if (!isatty(STDERR_FILENO)) {
        GTEST_SKIP() << "stderr is not a terminal, skipping width test";
    }

    const auto stream_width = get_stream_width(std::cerr);
    EXPECT_NE(stream_width, std::numeric_limits<size_t>::max());
}

// BSD 3-Clause License
//
// Copyright (c) 2026, Timo Behrend
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
