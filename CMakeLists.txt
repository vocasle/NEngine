cmake_minimum_required(VERSION 3.22)
project(NEngine LANGUAGES C)

set(CMAKE_C_STANDARD 11)

file(GLOB NENGINE_SOURCES CONFIGURE_DEPENDS src/*.c)

add_library(nengine SHARED ${NENGINE_SOURCES})
target_include_directories(nengine PUBLIC src math)
target_compile_options(nengine PRIVATE -Wall -Werror -Wextra -Wpedantic)
set_target_properties(nengine PROPERTIES LINKER_LANGUAGE C)
target_compile_definitions(nengine PRIVATE NE_EXPORT=1)

add_executable(app app/main.c)
target_compile_options(app PRIVATE -Wall -Werror -Wextra -Wpedantic)
target_link_libraries(app PRIVATE nengine nemath)
target_compile_definitions(app PRIVATE NE_EXPORT=0)

add_library(nemath SHARED math/nemath.h math/nemath.c)
target_include_directories(nemath PRIVATE src PUBLIC math)
target_compile_options(nemath PRIVATE -Wall -Werror -Wextra -Wpedantic)
target_compile_definitions(nemath PRIVATE NE_EXPORT=1)

find_package(Vulkan REQUIRED)
target_link_libraries(nengine PRIVATE ${Vulkan_LIBRARIES})
target_include_directories(nengine PRIVATE ${Vulkan_INCLUDE_DIR})
