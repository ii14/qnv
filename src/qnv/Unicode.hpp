#pragma once

#include <cstdint>
#include <string_view>
#include <QChar>

/// Invalid unicode codepoint
static constexpr uint32_t kInvalidCodepoint = 0x80000000;

/// Convert UTF-8 character to a unicode codepoint
/// @param[in] str UTF-8 string
/// @return Unicode codepoint, kInvalidCodepoint on error
[[nodiscard]] uint32_t utf8toUcs(std::string_view str);

/// Convert unicode codepoint to UTF-16
/// @param[in]  ucs Unicode character
/// @param[out] out Output buffer
/// @return Length of the buffer
unsigned ucsToUtf16(uint32_t ucs, QChar out[2]);

// vim: tw=100 sw=2 sts=2 et
