#pragma once

#include <stdexcept>

namespace msgpack_view {

struct Exception : public std::exception
{
  Exception() { }
  ~Exception() override { }
  Exception(const Exception&) = default;
  Exception(Exception&&) = default;
  Exception& operator=(const Exception&) = default;
  Exception& operator=(Exception&&) = default;
};

struct TypeError : msgpack_view::Exception
{
  TypeError() { }
  ~TypeError() override { }
  TypeError(const TypeError&) = default;
  TypeError(TypeError&&) = default;
  TypeError& operator=(const TypeError&) = default;
  TypeError& operator=(TypeError&&) = default;
  const char* what() const noexcept override { return "msgpack_view type error"; }
};

struct OutOfRange : msgpack_view::Exception
{
  OutOfRange() { }
  ~OutOfRange() override { }
  OutOfRange(const OutOfRange&) = default;
  OutOfRange(OutOfRange&&) = default;
  OutOfRange& operator=(const OutOfRange&) = default;
  OutOfRange& operator=(OutOfRange&&) = default;
  const char* what() const noexcept override { return "msgpack_view out of range"; }
};

} // namespace msgpack_view

// vim: tw=100 sw=2 sts=2 et
