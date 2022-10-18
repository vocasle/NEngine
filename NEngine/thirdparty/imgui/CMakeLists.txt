set(library_name imgui)

set(source_file_list
        src/imgui.cpp
        src/imgui_draw.cpp
        src/imgui_impl_dx11.cpp
        src/imgui_impl_dx12.cpp
        src/imgui_impl_win32.cpp
        src/imgui_tables.cpp
        src/imgui_widgets.cpp)

set(header_file_list
        include/imgui/imconfig.h
        include/imgui/imgui.h
        include/imgui/imgui_impl_dx11.h
        include/imgui/imgui_impl_dx12.h
        include/imgui/imgui_impl_win32.h
        include/imgui/imgui_internal.h
        include/imgui/imstb_rectpack.h
        include/imgui/imstb_textedit.h
        include/imgui/imstb_truetype.h)

add_library(${library_name} ${source_file_list} ${header_file_list})

target_include_directories(${library_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${library_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)