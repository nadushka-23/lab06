cmake_minimum_required(VERSION 3.10)
project(banking)

set(PRINT_VERSION_MAJOR 0)
set(PRINT_VERSION_MINOR 1)
set(PRINT_VERSION_PATCH 0)
set(PRINT_VERSION_TWEAK 0)
set(PRINT_VERSION
  ${PRINT_VERSION_MAJOR}.${PRINT_VERSION_MINOR}.${PRINT_VERSION_PATCH}.${PRINT_VERSION_TWEAK})
set(PRINT_VERSION_STRING "v${PRINT_VERSION}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(banking)

add_library(banking STATIC
    banking/Account.cpp
    banking/Transaction.cpp
)

add_executable(solver solver.cpp)
target_link_libraries(solver banking)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(third-party/gtest)
    add_executable(check tests/test_banking.cpp)
    target_link_libraries(check banking gtest_main gmock_main)
    add_test(NAME check COMMAND check)
endif()

install(TARGETS solver RUNTIME DESTINATION bin)

include(CPackConfig.cmake)
