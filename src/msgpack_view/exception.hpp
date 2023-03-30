#pragma once

#include <stdexcept>

namespace msgpack_view {

struct exception : public std::exception
{
  ~exception() override { }
};

struct type_error : msgpack_view::exception
{
  type_error() { }
  ~type_error() override { }
  const char* what() const noexcept override { return "msgpack_view type error"; }
};

struct out_of_range : msgpack_view::exception
{
  out_of_range() { }
  ~out_of_range() override { }
  const char* what() const noexcept override { return "msgpack_view out of range"; }
};

} // namespace msgpack_view

// vim: tw=100 sw=2 sts=2 et
