#pragma once

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#define EXPECT_SNAPSHOT_EQ(name, actual) \
    static_assert(sizeof(GTEST_STRINGIFY_(name)) > 1,                                          \
    "name must not be empty");                                                                 \
    expect_snapshot_eq(#name, std::string(TEST_PATH "/snapshots/") + #name + ".snap", actual); \

inline bool UPDATE_SNAPSHOTS = false;

void expect_snapshot_eq(const std::string &name, const std::filesystem::path &path, const std::string &actual);

class Snapshot {
public:
    Snapshot(std::string name, std::filesystem::path path)
        : _path(std::move(path)), _name(std::move(name)) {}

    void save(const std::string &data) const;

    [[nodiscard]] std::string load() const;

    [[nodiscard]] auto &path() const { return _path; }

    [[nodiscard]] auto &name() const { return _name; }

private:
    std::filesystem::path _path;
    std::string _name;
};

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