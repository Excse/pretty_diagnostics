#include "pretty_diagnostics/utils.hpp"

#include <iostream>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#endif

using namespace pretty_diagnostics;

std::string pretty_diagnostics::escape_string(const std::string_view input) {
    std::ostringstream output;

    for (const unsigned char current : input) {
        switch (current) {
            case '\n': {
                output << "\\n";
                break;
            }
            case '\t': {
                output << "\\t";
                break;
            }
            case '\r': {
                output << "\\r";
                break;
            }
            case '\"': {
                output << "\\\"";
                break;
            }
            case '\\': {
                output << "\\\\";
                break;
            }
            default: {
                if (std::isprint(current)) {
                    output << current;
                } else {
                    output << "\\x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(current) << std::dec;
                }
                break;
            }
        }
    }

    return output.str();
}

size_t pretty_diagnostics::get_stream_width(const std::ostream& stream) {
    int file_descriptor;
    if (&stream == &std::cout) {
        file_descriptor = STDOUT_FILENO;
    } else if (&stream == &std::cerr || &stream == &std::clog) {
        file_descriptor = STDERR_FILENO;
    } else {
        return std::numeric_limits<size_t>::max();
    }

#ifdef _WIN32
    HANDLE handle = (file_descriptor == STDOUT_FILENO) ? GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE);

    if (handle == INVALID_HANDLE_VALUE) {
        return std::numeric_limits<size_t>::max();
    }

    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

    const int result = GetConsoleScreenBufferInfo(handle, &screen_buffer_info);
    if (result == 0) {
        return std::numeric_limits<size_t>::max();
    }

    return static_cast<size_t>(screen_buffer_info.srWindow.Right - screen_buffer_info.srWindow.Left + 1);
#else
    if (!isatty(file_descriptor)) {
        return std::numeric_limits<size_t>::max();
    }

    winsize window_size{};

    const int result = ioctl(file_descriptor, TIOCGWINSZ, &window_size);
    if (result == -1) {
        return std::numeric_limits<size_t>::max();
    }

    return window_size.ws_col;
#endif
}

VisualChar pretty_diagnostics::get_visual_char(const std::string_view input, const size_t index) {
    if (index >= input.size()) {
        return { 0, 0 };
    }

    const unsigned char current = input[index];

    // ASCII
    if (current <= 0x7F) {
        return { 1, 1 };
    }

    // 2-byte UTF-8
    if ((current & 0xE0) == 0xC0 && index + 1 < input.size()) {
        return { 1, 2 };
    }

    // 3-byte UTF-8
    if ((current & 0xF0) == 0xE0 && index + 2 < input.size()) {
        return { 2, 3 };
    }

    // 4-byte UTF-8 (emoji, etc.)
    if ((current & 0xF8) == 0xF0 && index + 3 < input.size()) {
        return { 2, 4 };
    }

    // Invalid or truncated UTF-8
    return { 1, 1 };
}

size_t pretty_diagnostics::visual_width(const std::string_view input) {
    size_t visual_width = 0;

    for (size_t index = 0; index < input.size();) {
        auto [width, byte_count] = get_visual_char(input, index);
        visual_width += width;
        index += byte_count;
    }

    return visual_width;
}

size_t pretty_diagnostics::to_visual_column(const std::string_view line, const size_t byte_column) {
    size_t visual_column = 0;

    for (size_t index = 0; index < byte_column && index < line.size();) {
        auto [width, byte_count] = get_visual_char(line, index);
        visual_column += width;
        index += byte_count;
    }

    return visual_column;
}

size_t pretty_diagnostics::from_visual_column(const std::string_view line, const size_t visual_column) {
    size_t current_column = 0;
    size_t byte_column = 0;

    while (byte_column < line.size() && current_column < visual_column) {
        auto [width, byte_count] = get_visual_char(line, byte_column);

        if (current_column + width > visual_column) {
            break;
        }

        byte_column += byte_count;
        current_column += width;
    }

    return byte_column;
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
