cmake_minimum_required(VERSION 3.20)
project(NEngine)

set(CMAKE_CXX_STANDARD 20)

set(NENGINE_SOURCE_LIST
    NEngine/src/camera.cpp
    NEngine/src/main.cpp
    NEngine/src/vulkan_application.cpp
    NEngine/src/image.cpp)

set(NENGINE_HEADER_LIST
    NEngine/include/camera.h
    NEngine/include/vulkan_application.h
    NEngine/include/vertex.h
    NEngine/include/image.h
    NEngine/include/misc.h)

add_executable(nengine ${NENGINE_SOURCE_LIST} ${NENGINE_HEADER_LIST})

if (UNIX)
	find_package(SDL2 REQUIRED)
	find_package(Vulkan REQUIRED)
endif()

target_include_directories(nengine PRIVATE NEngine/include)

if (WIN32)
	target_link_directories(nengine PRIVATE NEngine/thirdparty/sdl2/lib/x64 $ENV{VK_SDK_PATH}/Lib)
	target_include_directories(nengine PRIVATE NEngine/thirdparty/sdl2/include $ENV{VK_SDK_PATH}/include)
else()
	target_include_directories(nengine PRIVATE ${Vulkan_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
endif()

add_subdirectory(NEngine/thirdparty/glm)
add_subdirectory(NEngine/thirdparty/imgui)
add_subdirectory(NEngine/thirdparty/stb_image)
add_subdirectory(NEngine/thirdparty/tinyobjloader)

if(WIN32)
	target_link_libraries(nengine PRIVATE glm::glm imgui stb_image tinyobjloader SDL2.lib vulkan-1.lib)
else()
	target_link_libraries(nengine PRIVATE glm::glm imgui stb_image tinyobjloader ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES})
endif()

find_program(GLSLC_EXE NAMES glslc REQUIRED)

# Taken from https://github.com/DanOlivier/Vulkan/blob/cmake-support/CMakeLists.txt
function(compile_shaders EXAMPLE_NAME)
    if(NOT ARGN)
        return()
    endif()
    message("SHADERS:")
    foreach(f ${ARGN})
        message("\t- ${f}")
    endforeach(f)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    foreach(SHADER ${ARGN})
        get_filename_component(FILE_NAME ${SHADER} NAME_WE)

        set(output_file ${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv)
        set(compiled_shaders ${compiled_shaders} ${output_file})
        set(compiled_shaders ${compiled_shaders} PARENT_SCOPE)
        add_custom_command(
            OUTPUT ${output_file}
            COMMAND ${GLSLC_EXE} -o ${output_file} ${CMAKE_SOURCE_DIR}/${SHADER}
            DEPENDS ${CMAKE_SOURCE_DIR}/${SHADER}
            COMMENT "Compiling shader ${output_file} with profile ${SHADER_PROFILE}"
        )
    endforeach()
    add_custom_target(shaders-${EXAMPLE_NAME} ALL DEPENDS ${compiled_shaders})
    add_dependencies(${EXAMPLE_NAME} shaders-${EXAMPLE_NAME})
endfunction()

set(SHADER_LIST NEngine/shaders/phong_fs.frag NEngine/shaders/phong_vs.vert)

compile_shaders(nengine ${SHADER_LIST})

target_compile_definitions(nengine PRIVATE 
    SHADERS_HOME_DIR="${CMAKE_CURRENT_BINARY_DIR}/shaders"
    RES_HOME_DIR="${CMAKE_SOURCE_DIR}/NEngine/res")

if(WIN32)
	add_custom_command(TARGET nengine POST_BUILD 
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
	    "${CMAKE_SOURCE_DIR}/NEngine/thirdparty/sdl2/lib/x64/SDL2.dll"              
	    $<TARGET_FILE_DIR:nengine>)
endif()
