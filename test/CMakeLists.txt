find_package(Catch2 3 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Test)
add_executable(test
    test_UserInput.cpp
    tostring.hpp
    main.cpp)
target_include_directories(test PRIVATE .)
target_link_libraries(test PRIVATE libqnv Catch2::Catch2 Qt5::Test)
