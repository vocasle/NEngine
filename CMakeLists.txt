cmake_minimum_required(VERSION 3.15)

project(NEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(FXC_EXE fxc REQUIRED)

add_subdirectory(NEngine)