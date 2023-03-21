#pragma once

#include <cstdint>
#include <string>

using Integer = int64_t;
using String = std::string;
using Boolean = bool;

struct Buffer
{
    static constexpr auto kMsgpackType = 0;
    constexpr Buffer() noexcept { }
    explicit constexpr Buffer(int64_t handle) noexcept : handle(handle) { }
    explicit constexpr operator int64_t() const noexcept { return handle; }
    friend constexpr bool operator==(Buffer a, Buffer b) noexcept { return a.handle == b.handle; }
    friend constexpr bool operator!=(Buffer a, Buffer b) noexcept { return a.handle != b.handle; }
    int64_t handle { 0 };
};

struct Window
{
    static constexpr auto kMsgpackType = 1;
    constexpr Window() noexcept { }
    explicit constexpr Window(int64_t handle) noexcept : handle(handle) { }
    explicit constexpr operator int64_t() const noexcept { return handle; }
    friend constexpr bool operator==(Window a, Window b) noexcept { return a.handle == b.handle; }
    friend constexpr bool operator!=(Window a, Window b) noexcept { return a.handle != b.handle; }
    int64_t handle { 0 };
};

struct Tabpage
{
    static constexpr auto kMsgpackType = 2;
    constexpr Tabpage() noexcept { }
    explicit constexpr Tabpage(int64_t handle) noexcept : handle(handle) { }
    explicit constexpr operator int64_t() const noexcept { return handle; }
    friend constexpr bool operator==(Tabpage a, Tabpage b) noexcept { return a.handle == b.handle; }
    friend constexpr bool operator!=(Tabpage a, Tabpage b) noexcept { return a.handle != b.handle; }
    int64_t handle { 0 };
};
