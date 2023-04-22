cmake_minimum_required(VERSION 3.20)
project(NEngine)

set(CMAKE_CXX_STANDARD 20)

set(NENGINE_SOURCE_LIST
    NEngine/src/camera.cpp
    NEngine/src/main.cpp
    NEngine/src/vulkan_application.cpp)

set(NENGINE_HEADER_LIST
    NEngine/include/camera.h
    NEngine/include/vulkan_application.h)

add_executable(nengine ${NENGINE_SOURCE_LIST} ${NENGINE_HEADER_LIST})

target_include_directories(nengine PRIVATE NEngine/include)