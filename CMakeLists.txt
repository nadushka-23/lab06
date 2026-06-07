cmake_minimum_required(VERSION 3.10)
project(banking)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем CMake, где искать заголовочные файлы .h класса banking
include_directories(banking)

add_library(banking STATIC
    banking/Account.cpp
    banking/Transaction.cpp
)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(third-party/gtest)
    add_executable(check tests/test_banking.cpp)
    target_link_libraries(check banking gtest_main gmock_main)
    add_test(NAME check COMMAND check)
endif()
