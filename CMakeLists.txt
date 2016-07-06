cmake_minimum_required(VERSION 2.8)
project(nrex CXX)

include(CheckCXXCompilerFlag)
function(append_cxx_flag_check flag)
    check_cxx_compiler_flag("${flag}" supported)
    if(supported)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
    endif()
endfunction()
append_cxx_flag_check("-Wall")
append_cxx_flag_check("-Wextra")
append_cxx_flag_check("-Werror")
append_cxx_flag_check("-pedantic")

add_library(nrex nrex.cpp nrex.hpp nrex_config.h)

add_executable(nrex-test test.cpp)
target_link_libraries(nrex-test nrex)

enable_testing()
add_test(NAME nrex-test WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" COMMAND nrex-test)
