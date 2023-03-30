#pragma once

#include <cstdint>
#include <string>

using Integer = int64_t;
using String = std::string;
using Boolean = bool;
using Float = double;

struct Buffer
{
  static constexpr auto kMsgpackType = 0;
  constexpr Buffer() noexcept { }
  explicit constexpr Buffer(int64_t handle) noexcept : mHandle(handle) { }
  explicit constexpr operator int64_t() const noexcept { return mHandle; }
  friend constexpr bool operator==(Buffer a, Buffer b) noexcept { return a.mHandle == b.mHandle; }
  friend constexpr bool operator!=(Buffer a, Buffer b) noexcept { return a.mHandle != b.mHandle; }
  int64_t mHandle { 0 };
};

struct Window
{
  static constexpr auto kMsgpackType = 1;
  constexpr Window() noexcept { }
  explicit constexpr Window(int64_t handle) noexcept : mHandle(handle) { }
  explicit constexpr operator int64_t() const noexcept { return mHandle; }
  friend constexpr bool operator==(Window a, Window b) noexcept { return a.mHandle == b.mHandle; }
  friend constexpr bool operator!=(Window a, Window b) noexcept { return a.mHandle != b.mHandle; }
  int64_t mHandle { 0 };
};

struct Tabpage
{
  static constexpr auto kMsgpackType = 2;
  constexpr Tabpage() noexcept { }
  explicit constexpr Tabpage(int64_t handle) noexcept : mHandle(handle) { }
  explicit constexpr operator int64_t() const noexcept { return mHandle; }
  friend constexpr bool operator==(Tabpage a, Tabpage b) noexcept { return a.mHandle == b.mHandle; }
  friend constexpr bool operator!=(Tabpage a, Tabpage b) noexcept { return a.mHandle != b.mHandle; }
  int64_t mHandle { 0 };
};

// vim: tw=100 sw=2 sts=2 et
