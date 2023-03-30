#pragma once

#include <msgpack.hpp>
#include "msgpack_view/exception.hpp"
#include "msgpack_view/iterator.hpp"

namespace msgpack_view {

struct View;
struct ArrayView;
struct MapView;
using StrView = std::string_view;

template <typename T>
struct convert;

struct View
{
  const msgpack::object& mRef;
  explicit View(const msgpack::object& v) : mRef(v) { }
  template <typename T>
  T as() const
  {
    return convert<T>::as(*this);
  }
  // TODO: is and to methods
};

inline std::ostream& operator<<(std::ostream& os, View view)
{
  return os << view.mRef;
}

struct ArrayView
{
  using value_type = View; // NOLINT(readability-identifier-naming)
  using size_type = size_t; // NOLINT(readability-identifier-naming)

  const msgpack::object_array& mRef;
  explicit ArrayView(const msgpack::object_array& v) : mRef(v) { }

  size_type size() const noexcept { return mRef.size; }
  bool empty() const noexcept { return mRef.size == 0; }
  value_type operator[](size_type i) const noexcept;
  value_type at(size_type i) const;

  // NOLINTNEXTLINE(readability-identifier-naming)
  using iterator = msgpack_view::Iterator<ArrayView, value_type>;
  iterator begin() const noexcept { return iterator { *this, 0 }; }
  iterator end() const noexcept { return iterator { *this, size() }; }
};

struct MapView
{
  using value_type = std::pair<View, View>; // NOLINT(readability-identifier-naming)
  using size_type = size_t; // NOLINT(readability-identifier-naming)

  const msgpack::object_map& mRef;
  explicit MapView(const msgpack::object_map& v) : mRef(v) { }

  size_type size() const noexcept { return mRef.size; }
  bool empty() const noexcept { return mRef.size == 0; }
  value_type operator[](size_type i) const noexcept;
  value_type at(size_type i) const;

  // NOLINTNEXTLINE(readability-identifier-naming)
  using iterator = msgpack_view::Iterator<MapView, value_type>;
  iterator begin() const noexcept { return iterator { *this, 0 }; }
  iterator end() const noexcept { return iterator { *this, size() }; }
};

inline View ArrayView::operator[](size_t i) const noexcept
{
  return View { mRef.ptr[i] };
}

inline View ArrayView::at(size_t i) const
{
  if (i < mRef.size)
    return View { mRef.ptr[i] };
  throw msgpack_view::OutOfRange {};
}

inline MapView::value_type MapView::operator[](size_t i) const noexcept
{
  return { View { mRef.ptr[i].key }, View { mRef.ptr[i].val } };
}

inline MapView::value_type MapView::at(size_t i) const
{
  if (i < mRef.size)
    return { View { mRef.ptr[i].key }, View { mRef.ptr[i].val } };
  throw msgpack_view::OutOfRange {};
}

} // namespace msgpack_view

// vim: tw=100 sw=2 sts=2 et
