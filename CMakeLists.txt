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


target_link_directories(nengine PRIVATE NEngine/thirdparty/sdl2/lib/x64 $ENV{VK_SDK_PATH}/Lib)
target_include_directories(nengine PRIVATE NEngine/thirdparty/sdl2/include $ENV{VK_SDK_PATH}/include)

add_subdirectory(NEngine/thirdparty/glm)
add_subdirectory(NEngine/thirdparty/imgui)
add_subdirectory(NEngine/thirdparty/stb_image)
add_subdirectory(NEngine/thirdparty/tinyobjloader)

target_link_libraries(nengine PRIVATE glm::glm imgui stb_image tinyobjloader SDL2.lib vulkan-1.lib)


