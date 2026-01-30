#pragma once

#include <ostream>

namespace pretty_diagnostics::color {

/**
 * @brief ANSI color codes and styles
 */
enum class Code {
    Reset = 0,
    Bold = 1,
    Dim = 2,
    Italic = 3,
    Underline = 4,
    Blink = 5,
    Reverse = 7,
    Hidden = 8,
    Strikethrough = 9,

    FgBlack = 30,
    FgRed = 31,
    FgGreen = 32,
    FgYellow = 33,
    FgBlue = 34,
    FgMagenta = 35,
    FgCyan = 36,
    FgWhite = 37,
    FgDefault = 39,

    BgBlack = 40,
    BgRed = 41,
    BgGreen = 42,
    BgYellow = 43,
    BgBlue = 44,
    BgMagenta = 45,
    BgCyan = 46,
    BgWhite = 47,
    BgDefault = 49,

    FgBrightBlack = 90,
    FgBrightRed = 91,
    FgBrightGreen = 92,
    FgBrightYellow = 93,
    FgBrightBlue = 94,
    FgBrightMagenta = 95,
    FgBrightCyan = 96,
    FgBrightWhite = 97,

    BgBrightBlack = 100,
    BgBrightRed = 101,
    BgBrightGreen = 102,
    BgBrightYellow = 103,
    BgBrightBlue = 104,
    BgBrightMagenta = 105,
    BgBrightCyan = 106,
    BgBrightWhite = 107,
};

/**
 * @brief Stream manipulator to output ANSI escape codes
 *
 * @param os Output stream to use the color
 * @param code ANSI code to output
 * @return Reference to the given stream
 */
std::ostream& operator<<(std::ostream& os, Code code);

/**
 * @brief A small structure containing all color codes and the text
 *        that should get printed
 *
 * @tparam N The number of color codes being used
 */
template <std::size_t N>
struct SyledText {
    std::array<Code, N> codes;
    std::string_view text;
};

/**
 * @brief Stream manipulator to output a styled text
 *
 * @tparam N The number of color codes being used
 * @param os The target output stream
 * @param text The styled text which is being outputted
 * @return Reference to the given stream
 */
template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const SyledText<N>& text) {
    for (const auto code : text.codes) {
        os << code;
    }
    os << text.text << Code::Reset;
    return os;
}

/**
 * @brief A variadic helper to make it easier to style text
 *
 * @param text The text that should get styled
 * @param codes All the codes to be used
 * @return A populated `StyledText` structure using the given parameters
 */
template <typename... Codes>
requires (std::conjunction_v<std::is_same<Codes, Code>...>)
[[nodiscard]] SyledText<sizeof...(Codes)> STYLE(std::string_view text, Codes... codes) {
    return SyledText<sizeof...(Codes)>{
        std::array<Code, sizeof...(Codes)>{codes...},
        text
    };
}

/**
 * @brief This will return a unique xalloc flag that can be used
 *        to check whether color is enabled or disabled for a specific
 *        stream
 *
 * @return A unique xalloc flag
 */
int color_enabled_index();

/**
 * @brief Enables or disables color output for a specific output stream
 *        If disabled, the stream manipulator will not output any escape codes
 *
 * @param os The output stream to be changed
 * @param enabled True to enable, false to disable
 */
void set_color_enabled(std::ostream& os, bool enabled);

/**
 * @brief Checks if color output is enabled for this stream
 *
 * @param os The output stream to be checked
 * @return True if enabled, false otherwise
 */
bool is_color_enabled(std::ostream& os);

} // namespace pretty_diagnostics::color

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
