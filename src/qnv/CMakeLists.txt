add_custom_target(gen_config
    COMMAND ${CMAKE_COMMAND}
        -DINPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/Config.cpp.in"
        -DOUTPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/generated/Config.cpp"
        -DRUNTIME_PATH="\"${RUNTIME_PATH}\""
        -P "${CMAKE_CURRENT_SOURCE_DIR}/Configure.cmake"
    DEPENDS Config.cpp.in Config.hpp
    BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/generated/Config.cpp")
set_source_files_properties(Config.cpp.in PROPERTIES HEADER_FILE_ONLY ON)

add_library(libqnv OBJECT
    ApiTypes.hpp
    Unicode.hpp
    Unicode.cpp
    Defs.hpp
    Grid.cpp
    Grid.hpp
    GridList.cpp
    GridList.hpp
    GridRenderer.cpp
    GridRenderer.hpp
    NvimProcess.cpp
    NvimProcess.hpp
    Session.cpp
    Session.hpp
    UserInput.cpp
    UserInput.hpp
    Config.hpp
    Config.cpp.in
    "${CMAKE_CURRENT_BINARY_DIR}/generated/Config.cpp")
add_dependencies(libqnv gen_config)

target_include_directories(libqnv PUBLIC ..)
target_link_libraries(libqnv PUBLIC
    Qt5::Core
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Widgets
    msgpack-cxx
    msgpack_view)

add_executable(qnv main.cpp)
target_link_libraries(qnv PRIVATE libqnv)
