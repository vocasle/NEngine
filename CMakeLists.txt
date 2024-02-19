cmake_minimum_required(VERSION 3.22)
project(NEngine LANGUAGES C)

set(CMAKE_C_STANDARD 11)

file(GLOB NENGINE_SOURCES CONFIGURE_DEPENDS src/*.c)

add_library(nengine SHARED ${NENGINE_SOURCES})
target_include_directories(nengine PUBLIC src)
target_compile_options(nengine PRIVATE -Wall -Werror -Wextra -Wpedantic)
set_target_properties(nengine PROPERTIES LINKER_LANGUAGE C)
target_compile_definitions(nengine PRIVATE NE_EXPORT=1)

add_executable(app app/main.c)
target_link_libraries(app PRIVATE nengine)
target_compile_definitions(app PRIVATE NE_EXPORT=0)
