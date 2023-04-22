set(library_name imgui)

set(source_file_list
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        backends/imgui_impl_vulkan.cpp
        backends/imgui_impl_sdl2.cpp
        imgui_tables.cpp
        imgui_widgets.cpp)

set(header_file_list
        imconfig.h
        imgui.h
        backends/imgui_impl_vulkan.h
        backends/imgui_impl_sdl2.h
        imgui_internal.h
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h)

add_library(${library_name} STATIC ${source_file_list} ${header_file_list})

target_include_directories(${library_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} backends
    PRIVATE ${CMAKE_SOURCE_DIR}/NEngine/thirdparty/sdl2/include
    PRIVATE $ENV{VK_SDK_PATH}/include)