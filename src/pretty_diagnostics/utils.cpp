#include "pretty_diagnostics/utils.hpp"

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

size_t pretty_diagnostics::visual_width(const std::string_view input) {
    size_t width = 0;

    for (size_t index = 0; index < input.size();) {
        const unsigned char current_char = input[index];
        if (current_char <= 0x7F) { // ASCII
            width += 1;
            index += 1;
        } else if ((current_char & 0xE0) == 0xC0) { // 2-byte sequence
            width += 1;
            index += 2;
        } else if ((current_char & 0xF0) == 0xE0) { // 3-byte sequence
            width += 2;
            index += 3;
        } else if ((current_char & 0xF8) == 0xF0) { // 4-byte sequence (Emojis, etc.)
            width += 2;
            index += 4;
        } else { // Invalid UTF-8, skip one byte
            width += 1;
            index += 1;
        }
    }

    return width;
}

size_t pretty_diagnostics::to_visual_column(const std::string_view line, const size_t byte_column) {
    size_t column = 0;

    for (size_t index = 0; index < byte_column && index < line.size();) {
        const unsigned char current_char = line[index];
        if (current_char <= 0x7F) { // ASCII
            column += 1;
            index += 1;
        } else if ((current_char & 0xE0) == 0xC0 && index + 1 < line.size()) { // 2-byte sequence
            column += 1;
            index += 2;
        } else if ((current_char & 0xF0) == 0xE0 && index + 2 < line.size()) { // 3-byte sequence
            column += 2;
            index += 3;
        } else if ((current_char & 0xF8) == 0xF0 && index + 3 < line.size()) { // 4-byte sequence (Emojis, etc.)
            column += 2;
            index += 4;
        } else { // Invalid UTF-8, skip one byte
            column += 1;
            index += 1;
        }
    }

    return column;
}

size_t pretty_diagnostics::from_visual_column(const std::string_view line, const size_t visual_column) {
    size_t current_column = 0;
    size_t byte_column = 0;

    while (byte_column < line.size() && current_column < visual_column) {
        const unsigned char current = line[byte_column];
        size_t char_width = 1;
        size_t char_bytes = 1;

        if (current <= 0x7F) { // ASCII
            char_width = 1;
            char_bytes = 1;
        } else if ((current & 0xE0) == 0xC0 && byte_column + 1 < line.size()) { // 2-byte sequence
            char_width = 1;
            char_bytes = 2;
        } else if ((current & 0xF0) == 0xE0 && byte_column + 2 < line.size()) { // 3-byte sequence
            char_width = 2;
            char_bytes = 3;
        } else if ((current & 0xF8) == 0xF0 && byte_column + 3 < line.size()) { // 4-byte sequence (Emojis, etc.)
            char_width = 2;
            char_bytes = 4;
        } else { // Invalid or truncated
            char_width = 1;
            char_bytes = 1;
        }

        if (current_column + char_width > visual_column) break;

        current_column += char_width;
        byte_column += char_bytes;
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
