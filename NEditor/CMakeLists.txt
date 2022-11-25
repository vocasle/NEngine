set(app_name NEditor)

set(source_file_list 
    src/main.cpp
    src/Editor.cpp)
set(header_file_list 
    include/NEditor/Editor.h)

add_executable(${app_name} WIN32 ${source_file_list} ${header_file_list})

target_include_directories(${app_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(${app_name} PRIVATE 
        NOMINMAX=1 
        UNICODE=1 
        _UNICODE=1 
        WITH_IMGUI=1)

target_link_libraries(${app_name} NEngine imgui DirectXMath)