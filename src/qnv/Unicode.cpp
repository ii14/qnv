#include "qnv/Unicode.hpp"

uint32_t utf8toUcs(std::string_view str)
{
  if (str.empty())
    return kInvalidCodepoint;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): reinterpret between signed/unsigned is fine
  const auto* d = reinterpret_cast<const uint8_t*>(str.data());
  if (d[0] < 0x80) {
    return d[0];
  } else if (d[0] < 0xC2) { // NOLINT(bugprone-branch-clone)
    return kInvalidCodepoint;
  } else if (d[0] < 0xE0) {
    if (str.size() < 2)
      return kInvalidCodepoint;
    if ((d[1] & 0xC0) != 0x80)
      return kInvalidCodepoint;
    uint32_t r = (uint32_t(d[0] & 0x1F) << 6) | (d[1] & 0x3F);
    return r >= 0x80 && r <= 0x7FF ? r : kInvalidCodepoint;
  } else if (d[0] < 0xF0) {
    if (str.size() < 3)
      return kInvalidCodepoint;
    if (((d[1] & 0xC0) != 0x80) | ((d[2] & 0xC0) != 0x80))
      return kInvalidCodepoint;
    uint32_t r = (uint32_t(d[0] & 0x0F) << 12) | (uint32_t(d[1] & 0x3F) << 6) | (d[2] & 0x3F);
    if (r >= 0xD800 && r <= 0xDFFF)
      return kInvalidCodepoint;
    return r >= 0x800 && r <= 0xFFFF ? r : kInvalidCodepoint;
  } else if (d[0] < 0xF8) {
    if (str.size() < 4)
      return kInvalidCodepoint;
    if (((d[1] & 0xC0) != 0x80) | ((d[2] & 0xC0) != 0x80) | ((d[3] & 0xC0) != 0x80))
      return kInvalidCodepoint;
    uint32_t r = (uint32_t(d[0] & 0x07) << 18) | (uint32_t(d[1] & 0x3F) << 12)
        | (uint32_t(d[2] & 0x3F) << 6) | (d[3] & 0x3F);
    return r >= 0x10000 && r <= 0x10FFFF ? r : kInvalidCodepoint;
  } else {
    return kInvalidCodepoint;
  }
}

unsigned ucsToUtf16(uint32_t ucs, QChar out[2])
{
  if (ucs <= 0xFFFF) {
    out[0] = ucs;
    out[1] = 0;
    return 1;
  } else {
    ucs -= 0x10000;
    out[0] = (ucs >> 10) + 0xD800;
    out[1] = (ucs & 0x3FF) + 0xDC00;
    return 2;
  }
}

// vim: tw=100 sw=2 sts=2 et
