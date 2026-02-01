#pragma once

#include <iomanip>
#include <string>
#include <string_view>

namespace pretty_diagnostics {

/**
 * @brief Escapes control characters and quotes in a string for safe display.
 *        For example, converts newlines to "\\n" and tabs to "\\t"
 *
 * @param input Input string view to escape
 *
 * @return A newly allocated escaped string suitable for diagnostics output
 */
std::string escape_string(std::string_view input);

/**
 * @brief Returns the maximum of a single value.
 *
 * @tparam T Type of the value (supports <)
 * @param first The value
 * @return The value itself
 */
template <typename T>
T max(T first) {
    return first;
}

/**
 * @brief Returns the maximum of multiple values.
 *
 * @tparam T Type of the first argument (supports <)
 * @tparam Ts Types of remaining arguments
 * @param first First argument
 * @param rest Remaining arguments
 * @return Maximum value among all arguments
 */
template <typename T, typename... Ts>
T max(T first, Ts... rest) {
    T rest_max = max(rest...);
    return (first < rest_max) ? rest_max : first;
}

/**
 * @brief Returns the minimum of a single value
 *
 * @tparam T Type of the value (supports <)
 * @param first The value
 * @return The value itself
 */
template <typename T>
T min(T first) {
    return first;
}

/**
 * @brief Returns the minimum of multiple values
 *
 * @tparam T Type of the first argument (supports <)
 * @tparam Ts Types of remaining arguments
 * @param first First argument
 * @param rest Remaining arguments
 * @return Minimum value among all arguments
 */
template <typename T, typename... Ts>
T min(T first, Ts... rest) {
    T rest_min = min(rest...);
    return (rest_min < first) ? rest_min : first;
}

size_t get_stream_width(const std::ostream &stream);

/**
 * @brief A structure to contian the return values from `get_visual_char`
 */
struct VisualChar {
    size_t visual_width;
    size_t byte_count;
};

/**
 * @brief Returns the visual width and byte count of a given UTF8 character
 *
 * @param input The input stringview that contains the UTF8 character
 * @param index The index at which the character is located
 * @return
 */
[[nodiscard]] VisualChar get_visual_char(std::string_view input, size_t index);

/**
 * @brief Calculates the visual display width of a UTF-8 string (for terminal display)
 *
 * ASCII characters count as 1, most 3- and 4-byte characters (CJK, emojis) as 2.
 * Invalid UTF-8 bytes are counted as width 1.
 *
 * @param input Input UTF-8 string view
 *
 * @return Visual width in terminal columns
 */
size_t visual_width(std::string_view input);

/**
 * @brief Returns the visual column of a specific byte column in a UTF-8 string
 *
 * @param line Input UTF-8 string view (usually a single line)
 * @param byte_column 0-based byte column into @p line
 *
 * @return 0-based visual column index
 */
size_t to_visual_column(std::string_view line, size_t byte_column);

/**
 * @brief Maps a visual column to a UTF-8 byte index in a line
 *
 * @param line Input UTF-8 string view (usually a single line)
 * @param visual_column 0-based visual column into @p line
 *
 * @return 0-based visual column index
 */
size_t from_visual_column(std::string_view line, size_t visual_column);

} // namespace pretty_diagnostics

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
