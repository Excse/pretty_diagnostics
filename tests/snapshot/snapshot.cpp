#include "snapshot.hpp"

#include <fstream>
#include <sstream>

void expect_snapshot_eq(const std::string& name, const std::filesystem::path& path, const std::string& actual) {
    const auto snapshot = Snapshot(name, path);

#ifdef UPDATE_SNAPSHOTS
    snapshot.save(actual);
    SUCCEED() << "Snapshot \"" << snapshot.name() << "\" updated: " << snapshot.path();
#else
    const auto expected = snapshot.load();
    EXPECT_EQ(expected, actual) << "Snapshot \"" << snapshot.name() << "\" mismatch: " << snapshot.path();
    SUCCEED() << "Snapshot \"" << snapshot.name() << "\" matched: " << snapshot.path();
#endif
}

void Snapshot::save(const std::string& data) const {
    const auto directory = _path.parent_path();
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }

    std::ofstream file(_path);
    if (!file.is_open()) {
        throw std::runtime_error("Snapshot::save(): failed to open file: " + _path.string());
    }

    file << data;
}

std::string Snapshot::load() const {
    std::ifstream file(_path);
    if (!file.is_open()) {
        throw std::runtime_error("Snapshot::load(): failed to open file: " + _path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
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
