#include "pretty_diagnostics/color.hpp"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace pretty_diagnostics::color;
using namespace pretty_diagnostics;

std::ostream& color::operator<<(std::ostream& os, Code code) {
    if (is_color_enabled(os)) {
        os << "\033[" << static_cast<int>(code) << "m";
    }
    return os;
}

int color::color_enabled_index() {
    static int COLOR_INDEX = std::ios_base::xalloc();
    return COLOR_INDEX;
}

void color::set_color_enabled(std::ostream& os, const bool enabled) {
    os.iword(color_enabled_index()) = enabled ? 1 : 0;
}

bool color::is_color_enabled(std::ostream& os) {
    return os.iword(color_enabled_index()) != 0;
}

bool color::is_colorable(const std::ostream& os) {
#ifdef _WIN32
    HANDLE handle = nullptr;

    if (&os == &std::cout) {
        handle = GetStdHandle(STD_OUTPUT_HANDLE);
    } else if (&os == &std::cerr) {
        handle = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        return false;
    }

    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD mode = 0;
    if (!GetConsoleMode(handle, &mode)) {
        return false;
    }

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(handle, mode);
    return true;
#else
    if (&os == &std::cout) {
        return ::isatty(STDOUT_FILENO);
    }
    if (&os == &std::cerr) {
        return ::isatty(STDERR_FILENO);
    }
    return false;
#endif
}

void color::auto_enable_color(std::ostream& os) {
    set_color_enabled(os, is_colorable(os));
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
