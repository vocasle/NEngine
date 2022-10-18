set(library_name stb_image_write)

set(source_file_list
        src/stb_image_write.cpp)

set(header_file_list
        include/stb_image_write/stb_image_write.h)

add_library(${library_name} ${source_file_list} ${header_file_list})

target_include_directories(${library_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)