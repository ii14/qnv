#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace msgpack_view {

template <typename T, typename U>
struct Iterator
{
  T mV;
  size_t mI;

  // NOLINTBEGIN(readability-identifier-naming)
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = ptrdiff_t;
  using value_type = U;
  using reference = value_type;
  struct pointer
  {
    value_type mV;
    explicit pointer(value_type view) noexcept : mV(view) { }
    value_type operator*() noexcept { return mV; }
    value_type* operator->() noexcept { return &mV; }
  };
  // NOLINTEND(readability-identifier-naming)

  explicit Iterator(T view, size_t index) noexcept : mV(view), mI(index) { }

  reference operator*() const noexcept { return mV[mI]; }
  pointer operator->() const noexcept { return pointer { mV[mI] }; }
  reference operator[](difference_type j) const noexcept { return mV[mI + j]; }

  bool operator==(Iterator b) const noexcept { return mI == b.mI; }
  bool operator!=(Iterator b) const noexcept { return mI != b.mI; }
  bool operator<(Iterator b) const noexcept { return mI < b.mI; }
  bool operator<=(Iterator b) const noexcept { return mI <= b.mI; }
  bool operator>(Iterator b) const noexcept { return mI > b.mI; }
  bool operator>=(Iterator b) const noexcept { return mI >= b.mI; }

  // clang-format off
  Iterator& operator++() noexcept { ++mI; return *this; }
  Iterator operator++(int) noexcept { Iterator n = *this; ++mI; return n; }
  Iterator& operator--() noexcept { --mI; return *this; }
  Iterator operator--(int) noexcept { Iterator n = *this; --mI; return n; }

  Iterator& operator+=(difference_type j) noexcept { mI += j; return *this; }
  Iterator& operator-=(difference_type j) noexcept { mI -= j; return *this; }
  // clang-format on

  Iterator operator+(difference_type j) const noexcept { return Iterator { mV, mI + j }; }
  Iterator operator-(difference_type j) const noexcept { return Iterator { mV, mI - j }; }
  difference_type operator-(Iterator j) const noexcept { return mI - j.i; }
};

} // namespace msgpack_view

// vim: tw=100 sw=2 sts=2 et
