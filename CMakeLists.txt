cmake_minimum_required(VERSION 3.15)

project(NEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(FXC_EXE fxc REQUIRED)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
        set(CMAKE_BUILD_TYPE Debug)
endif()

add_subdirectory(NEngine)
add_subdirectory(Sandbox)
add_subdirectory(NEditor)