add_executable(gen_api gen_api.cpp)
target_include_directories(gen_api PRIVATE .)
target_link_libraries(gen_api PRIVATE Qt5::Core msgpack-cxx msgpack_view)
