add_library(msgpack_view INTERFACE
    exception.hpp
    iterator.hpp
    view.hpp
    convert.hpp
    msgpack_view.hpp)
target_include_directories(msgpack_view INTERFACE ..)
target_link_libraries(msgpack_view INTERFACE msgpack-cxx)
