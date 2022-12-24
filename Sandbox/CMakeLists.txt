set(app_name Sandbox)

set(source_file_list
        include/Sandbox/Game.h)

set(header_file_list
        src/Game.cpp
        src/Main.cpp)

add_executable(${app_name} 
        ${source_file_list} 
        ${header_file_list})

target_include_directories(${app_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(${app_name} PRIVATE 
        NOMINMAX=1 
        UNICODE=1 
        _UNICODE=1 
        WITH_IMGUI=1)

target_link_libraries(${app_name} NEngine imgui)

target_compile_options(${app_name}  PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
                -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:
                /W4>)