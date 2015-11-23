cmake_minimum_required(VERSION 2.8)
project(nrex CXX)

add_library(nrex nrex.cpp nrex.hpp nrex_config.h)

add_executable(nrex-test test.cpp)
target_link_libraries(nrex-test nrex)

enable_testing()
add_test(NAME nrex-test WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" COMMAND nrex-test)
