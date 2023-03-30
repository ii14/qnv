#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace msgpack_view {

template <typename T, typename U>
struct iterator
{
  T view;
  size_t i;

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = ptrdiff_t;
  using value_type = U;
  using reference = value_type;
  struct pointer
  {
    value_type view;
    explicit pointer(value_type view) noexcept : view(view) { }
    value_type operator*() noexcept { return view; }
    value_type* operator->() noexcept { return &view; }
  };

  explicit iterator(T view, size_t index) noexcept : view(view), i(index) { }

  reference operator*() const noexcept { return view[i]; }
  pointer operator->() const noexcept { return pointer { view[i] }; }
  reference operator[](difference_type j) const noexcept { return view[i + j]; }

  bool operator==(iterator b) const noexcept { return i == b.i; }
  bool operator!=(iterator b) const noexcept { return i != b.i; }
  bool operator<(iterator b) const noexcept { return i < b.i; }
  bool operator<=(iterator b) const noexcept { return i <= b.i; }
  bool operator>(iterator b) const noexcept { return i > b.i; }
  bool operator>=(iterator b) const noexcept { return i >= b.i; }

  // clang-format off
  iterator& operator++() noexcept { ++i; return *this; }
  iterator operator++(int) noexcept { iterator n = *this; ++i; return n; }
  iterator& operator--() noexcept { --i; return *this; }
  iterator operator--(int) noexcept { iterator n = *this; --i; return n; }

  iterator& operator+=(difference_type j) noexcept { i += j; return *this; }
  iterator& operator-=(difference_type j) noexcept { i -= j; return *this; }
  // clang-format on

  iterator operator+(difference_type j) const noexcept { return iterator { view, i + j }; }
  iterator operator-(difference_type j) const noexcept { return iterator { view, i - j }; }
  difference_type operator-(iterator j) const noexcept { return i - j.i; }
};

} // namespace msgpack_view

// vim: tw=100 sw=2 sts=2 et
