#pragma once

#include "msgpack_view/view.hpp"
#include <array>
#include <tuple>
#include <utility>
#include <vector>
#include "qnv/ApiTypes.hpp"

namespace msgpack_view {

template <>
struct convert<bool>
{
  static bool as(View view)
  {
    if (view.mRef.type == msgpack::type::BOOLEAN)
      return view.mRef.via.boolean;
    throw msgpack_view::TypeError {};
  }
};

template <>
struct convert<uint64_t>
{
  static uint64_t as(View view)
  {
    if (view.mRef.type == msgpack::type::POSITIVE_INTEGER)
      return view.mRef.via.u64;
    throw msgpack_view::TypeError {};
  }
};

template <>
struct convert<int64_t>
{
  static int64_t as(View view)
  {
    if (view.mRef.type == msgpack::type::POSITIVE_INTEGER)
      return view.mRef.via.u64;
    if (view.mRef.type == msgpack::type::NEGATIVE_INTEGER)
      return view.mRef.via.i64;
    throw msgpack_view::TypeError {};
  }
};

template <>
struct convert<double>
{
  static double as(View view)
  {
    if (view.mRef.type == msgpack::type::FLOAT64)
      return view.mRef.via.f64;
    if (view.mRef.type == msgpack::type::FLOAT32) // is this even needed?
      return view.mRef.via.f64;
    throw msgpack_view::TypeError {};
  }
};

template <>
struct convert<ArrayView>
{
  static ArrayView as(View view)
  {
    if (view.mRef.type == msgpack::type::ARRAY)
      return ArrayView { view.mRef.via.array };
    throw msgpack_view::TypeError {};
  }
};

template <>
struct convert<MapView>
{
  static MapView as(View view)
  {
    if (view.mRef.type == msgpack::type::MAP)
      return MapView { view.mRef.via.map };
    throw msgpack_view::TypeError {};
  }
};

template <>
struct convert<std::string_view>
{
  static std::string_view as(View view)
  {
    if (view.mRef.type == msgpack::type::STR)
      return { view.mRef.via.str.ptr, view.mRef.via.str.size };
    throw msgpack_view::TypeError {};
  }
};

template <>
struct convert<std::string>
{
  static std::string as(View view)
  {
    if (view.mRef.type == msgpack::type::STR)
      return { view.mRef.via.str.ptr, view.mRef.via.str.size };
    throw msgpack_view::TypeError {};
  }
};

template <>
struct convert<Buffer>
{
  static Buffer as(View view)
  {
    if (view.mRef.type != msgpack::type::EXT)
      throw msgpack_view::TypeError {};
    if (view.mRef.via.ext.type() != Buffer::kMsgpackType)
      throw msgpack_view::TypeError {};
    auto d = msgpack::unpack(view.mRef.via.ext.data(), view.mRef.via.ext.size);
    return Buffer { View { d.get() }.as<int64_t>() };
  }
};

template <>
struct convert<Window>
{
  static Window as(View view)
  {
    if (view.mRef.type != msgpack::type::EXT)
      throw msgpack_view::TypeError {};
    if (view.mRef.via.ext.type() != Window::kMsgpackType)
      throw msgpack_view::TypeError {};
    auto d = msgpack::unpack(view.mRef.via.ext.data(), view.mRef.via.ext.size);
    return Window { View { d.get() }.as<int64_t>() };
  }
};

template <>
struct convert<Tabpage>
{
  static Tabpage as(View view)
  {
    if (view.mRef.type != msgpack::type::EXT)
      throw msgpack_view::TypeError {};
    if (view.mRef.via.ext.type() != Tabpage::kMsgpackType)
      throw msgpack_view::TypeError {};
    auto d = msgpack::unpack(view.mRef.via.ext.data(), view.mRef.via.ext.size);
    return Tabpage { View { d.get() }.as<int64_t>() };
  }
};

template <typename T, typename U>
struct convert<std::pair<T, U>>
{
  static std::pair<T, U> as(View view)
  {
    auto arr = view.as<ArrayView>();
    if (arr.size() < 2)
      throw msgpack_view::TypeError {};
    return { arr[0].as<T>(), arr[1].as<U>() };
  }
};

template <typename T>
struct convert<std::vector<T>>
{
  static std::vector<T> as(View view)
  {
    auto arr = view.as<ArrayView>();
    std::vector<T> res;
    res.reserve(arr.size());
    for (auto item : arr)
      res.push_back(item.as<T>());
    return res;
  }
};

template <typename T, size_t N>
struct convert<std::array<T, N>>
{
  static std::array<T, N> as(View view)
  {
    auto arr = view.as<ArrayView>();
    if (arr.size() < N)
      throw msgpack_view::TypeError {};
    std::array<T, N> res;
    for (size_t i = 0; i < N; ++i)
      res[i] = arr[i].as<T>();
    return res;
  }
};

// template <typename... T> struct convert<std::tuple<T...>> {
//     using value_type = std::tuple<T...>;
//     static value_type as(object_view view) {
//         auto arr = view.as<array_view>();
//         if (arr.size() < std::tuple_size_v<value_type>)
//             throw msgpack_view::type_error { };
//         // TODO: fold expression
//     }
// };

} // namespace msgpack_view

// vim: tw=100 sw=2 sts=2 et
