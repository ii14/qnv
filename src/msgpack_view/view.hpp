#pragma once

#include <msgpack.hpp>
#include "msgpack_view/exception.hpp"
#include "msgpack_view/iterator.hpp"

namespace msgpack_view {

struct object_view;
struct array_view;
struct map_view;
using str_view = std::string_view;

template <typename T>
struct convert;

struct object_view
{
  const msgpack::object& ref;
  object_view(const msgpack::object& v) : ref(v) { }
  template <typename T>
  T as() const
  {
    return convert<T>::as(*this);
  }
  // TODO: is and to methods
};

inline std::ostream& operator<<(std::ostream& os, object_view view)
{
  return os << view.ref;
}

struct array_view
{
  using value_type = object_view;
  using size_type = size_t;

  const msgpack::object_array& ref;
  array_view(const msgpack::object_array& v) : ref(v) { }

  size_type size() const noexcept { return ref.size; }
  bool empty() const noexcept { return ref.size == 0; }
  value_type operator[](size_type i) const noexcept;
  value_type at(size_type i) const;

  using iterator = msgpack_view::iterator<array_view, value_type>;
  iterator begin() const noexcept { return iterator { *this, 0 }; }
  iterator end() const noexcept { return iterator { *this, size() }; }
};

struct map_view
{
  using value_type = std::pair<object_view, object_view>;
  using size_type = size_t;

  const msgpack::object_map& ref;
  map_view(const msgpack::object_map& v) : ref(v) { }

  size_type size() const noexcept { return ref.size; }
  bool empty() const noexcept { return ref.size == 0; }
  value_type operator[](size_type i) const noexcept;
  value_type at(size_type i) const;

  using iterator = msgpack_view::iterator<map_view, value_type>;
  iterator begin() const noexcept { return iterator { *this, 0 }; }
  iterator end() const noexcept { return iterator { *this, size() }; }
};

inline object_view array_view::operator[](size_t i) const noexcept
{
  return ref.ptr[i];
}

inline object_view array_view::at(size_t i) const
{
  if (i < ref.size)
    return ref.ptr[i];
  throw msgpack_view::out_of_range {};
}

inline map_view::value_type map_view::operator[](size_t i) const noexcept
{
  return { ref.ptr[i].key, ref.ptr[i].val };
}

inline map_view::value_type map_view::at(size_t i) const
{
  if (i < ref.size)
    return { ref.ptr[i].key, ref.ptr[i].val };
  throw msgpack_view::out_of_range {};
}

} // namespace msgpack_view

// vim: tw=100 sw=2 sts=2 et
