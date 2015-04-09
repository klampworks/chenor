cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -Wextra -I/usr/include/botan-1.10/")
set(CMAKE_CXX_COMPILER_ID "Clang")
project(chenor)

add_executable(
    utest
    utest.cpp
)
target_link_libraries(utest CppUTest)
target_link_libraries(utest botan)

add_executable(
    chenor_utest
    chenor_utest.cpp
    chenor.cpp
    mock_write.cpp
)
target_link_libraries(chenor_utest CppUTest)
target_link_libraries(chenor_utest CppUTestExt)
