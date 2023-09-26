cmake_minimum_required(VERSION 3.20)
project(NEngine LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(thirdparty)

add_executable(NEngine src/main.cpp)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
target_link_libraries(NEngine PRIVATE 
	imgui
	${SDL2_LIBRARIES}
	${Vulkan_LIBRARIES})
target_include_directories(NEngine PRIVATE
	${SDL2_INCLUDE_DIR}
	${Vulkan_INCLUDE_DIR})

