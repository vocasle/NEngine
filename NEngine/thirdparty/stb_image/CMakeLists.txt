set(library_name stb_image)

set(source_file_list
        src/stb_image.cpp)

set(header_file_list
        include/stb_image/stb_image.h)

add_library(${library_name} ${source_file_list} ${header_file_list})

target_include_directories(${library_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)