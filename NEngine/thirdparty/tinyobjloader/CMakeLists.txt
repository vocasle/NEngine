add_library(tinyobjloader STATIC tiny_obj_loader.h tiny_obj_loader.cpp)
target_include_directories(tinyobjloader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
