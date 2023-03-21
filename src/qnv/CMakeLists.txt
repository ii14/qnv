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
    UserInput.hpp)
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
