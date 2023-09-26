add_library(imgui STATIC imgui.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_draw.cpp
    backends/imgui_impl_sdl2.cpp
    backends/imgui_impl_vulkan.cpp
    imgui_demo.cpp
    imgui.h
    backends/imgui_impl_sdl2.h
    backends/imgui_impl_vulkan.h)
target_include_directories(imgui PUBLIC ./ ./backends)

find_package(SDL2 REQUIRED)
target_include_directories(imgui PRIVATE
	${SDL2_INCLUDE_DIR})

