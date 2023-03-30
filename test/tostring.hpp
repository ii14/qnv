#pragma once

#include <catch2/catch_tostring.hpp>
#include <QDebug>
#include <QString>

namespace detail {

template <typename T>
inline std::string qDebugToStdString(const T& v)
{
  QString s;
  QDebug(&s).nospace() << v;
  return s.toStdString();
}

} // namespace detail

#define CATCH_QT_TOSTRING(TYPE) \
  namespace Catch { \
  template <> \
  struct StringMaker<TYPE> \
  { \
    static std::string convert(const TYPE& v) \
    { \
      return detail::qDebugToStdString(v); \
    } \
  }; \
  }

CATCH_QT_TOSTRING(QString)

// vim: tw=100 sw=2 sts=2 et
