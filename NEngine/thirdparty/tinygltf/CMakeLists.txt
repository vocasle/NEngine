set(library_name tinygltf)

set(source_file_list
        src/tiny_gltf.cpp)

set(header_file_list
        include/tinygltf/tiny_gltf.h)

add_library(${library_name} ${source_file_list} ${header_file_list})

target_include_directories(${library_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${library_name} PRIVATE stb_image stb_image_write)