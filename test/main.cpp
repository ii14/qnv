#include <catch2/catch_session.hpp>
#include <QCoreApplication>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  return Catch::Session().run(argc, argv);
}

extern "C" const char* __ubsan_default_options() // NOLINT
{
  return "print_stacktrace=1";
}

// vim: tw=100 sw=2 sts=2 et
