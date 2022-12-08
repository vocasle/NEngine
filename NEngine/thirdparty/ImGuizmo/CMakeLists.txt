set(library_name ImGuizmo)

set(source_file_list
    src/GraphEditor.cpp
    src/ImCurveEdit.cpp
    src/ImGradient.cpp
    src/ImGuizmo.cpp
    src/ImSequencer.cpp)

set(header_file_list
    include/ImGuizmo/GraphEditor.h
    include/ImGuizmo/ImCurveEdit.h
    include/ImGuizmo/ImGradient.h
    include/ImGuizmo/ImGuizmo.h
    include/ImGuizmo/ImSequencer.h
    include/ImGuizmo/ImZoomSlider.h)

add_library(${library_name} ${source_file_list} ${header_file_list})
target_link_libraries(${library_name} PUBLIC imgui)

target_include_directories(${library_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/include/imgui)
target_include_directories(${library_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${library_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/ImGuizmo)