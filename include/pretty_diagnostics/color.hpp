#pragma once

#include <ostream>

namespace pretty_diagnostics::color {

/**
 * @brief ANSI escape codes for text styling and coloring
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
 * @brief Outputs a single ANSI escape code to the given stream
 *
 * If color output is disabled for the stream, this operation produces no output
 *
 * @param os Target output stream
 * @param code ANSI style or color code to emit
 * @return Reference to the given stream
 */
std::ostream& operator<<(std::ostream& os, Code code);

/**
 * @brief Holds a piece of text together with one or more ANSI style codes
 *
 * This is a lightweight proxy type used to apply multiple styles to a text
 * segment and automatically reset formatting after output
 *
 * @tparam N Number of ANSI codes applied to the text
 */
template <std::size_t N>
struct SyledText {
    std::array<Code, N> codes;
    std::string_view text;
};

/**
 * @brief Outputs styled text to the given stream
 *
 * All style codes are emitted in order, followed by the text and a final
 * reset code to restore default formatting
 *
 * @tparam N Number of ANSI codes applied to the text
 * @param os Target output stream
 * @param text Styled text to be written
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
 * @brief Creates a styled text object from a text and a set of ANSI codes
 *
 * This helper enables concise and expressive syntax for applying multiple
 * styles to a single text segment
 *
 * @param text Text to be styled
 * @param codes ANSI style and color codes to apply
 * @return A `SyledText` instance containing the provided styles and text
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
 * @brief Returns a unique `std::ios_base::xalloc` index used to store
 *        per-stream color enable state
 *
 * @return Index used for accessing the stream-local color flag
 */
int color_enabled_index();

/**
 * @brief Enables or disables color output for a specific output stream
 *
 * When disabled, no ANSI escape codes will be emitted for that stream
 *
 * @param os Output stream to modify
 * @param enabled Whether color output should be enabled
 */
void set_color_enabled(std::ostream& os, bool enabled);

/**
 * @brief Checks whether color output is enabled for the given stream
 *
 * @param os Output stream to query
 * @return True if color output is enabled, false otherwise
 */
bool is_color_enabled(std::ostream& os);

/**
 * @brief Checks whether the given output stream supports colored output.
 *
 * Performs a platform-specific best-effort check to determine whether the
 * stream is connected to a terminal capable of displaying ANSI escape codes.
 *
 * @param os Output stream to check
 * @return True if the stream likely supports color output
 */
bool is_colorable(const std::ostream& os);

/**
 * @brief Enables or disables color output for a stream based on its capability.
 *
 * Calls `is_colorable(os)` and enables color output if supported, otherwise
 * disables it.
 *
 * @param os Output stream to initialize
 */
void auto_enable_color(std::ostream& os);

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
