set(library_name NEngine)

set(source_file_list
        src/Helpers/Camera.cpp
        src/Helpers/DeviceResources.cpp
        src/Helpers/DynamicConstBuffer.cpp
        src/Helpers/DynamicCubeMap.cpp
        src/Helpers/GLTFLoader.cpp
        src/Helpers/InputLayoutOld.cpp
        src/Helpers/MeshGenerator.cpp
        src/Helpers/objloader.cpp
        src/Helpers/ParticleSystem.cpp
        src/Helpers/Renderer.cpp
        src/Helpers/ShaderManager.cpp
        src/Helpers/ShadowMap.cpp
        src/Helpers/VertexBuffer.cpp
        src/Input/Keyboard.cpp
        src/Input/Mouse.cpp
        src/Math/Math.cpp
        src/Renderer/IndexBuffer.cpp
        src/Renderer/InputLayout.cpp
        src/Renderer/MeshPrimitive.cpp
        src/Renderer/Mesh.cpp
        src/Utils/Utils.cpp
        src/Renderer/VertexShader.cpp
        src/Renderer/PixelShader.cpp
        src/Renderer/BasePass.cpp
        src/Renderer/Texture.cpp
        src/Renderer/RasterizerState.cpp
        src/Helpers/Transform.cpp
        src/Renderer/Sampler.cpp)

set(header_file_list
        include/NEngine/Helpers/Buffer.h
        include/NEngine/Helpers/Camera.h
        include/NEngine/Helpers/DeviceResources.h
        include/NEngine/Helpers/DynamicConstBuffer.h
        include/NEngine/Helpers/DynamicCubeMap.h
        include/NEngine/Helpers/GLTFLoader.h
        include/NEngine/Helpers/InputLayout.h
        include/NEngine/Helpers/LightHelper.h
        include/NEngine/Helpers/MeshGenerator.h
        include/NEngine/Helpers/objloader.h
        include/NEngine/Helpers/ParticleSystem.h
        include/NEngine/Helpers/Renderer.h
        include/NEngine/Helpers/ShaderManager.h
        include/NEngine/Helpers/ShadowMap.h
        include/NEngine/Input/Keyboard.h
        include/NEngine/Input/Mouse.h
        include/NEngine/Math/Math.h
        include/NEngine/Renderer/Bindable.h
        include/NEngine/Renderer/Drawable.h
        include/NEngine/Renderer/IndexBuffer.h
        include/NEngine/Renderer/InputLayout.h
        include/NEngine/Renderer/MeshPrimitive.h
        include/NEngine/Renderer/Mesh.h
        include/NEngine/Renderer/VertexBuffer.h
        include/NEngine/Utils/Image.h
        include/NEngine/Utils/Timer.h
        include/NEngine/Utils/Utils.h
        include/NEngine/Renderer/VertexShader.h
        include/NEngine/Renderer/PixelShader.h
        include/NEngine/Renderer/BasePass.h
        include/NEngine/Renderer/Texture.h
        include/NEngine/Renderer/RasterizerState.h
        include/NEngine/Helpers/Transform.h
        include/NEngine/Renderer/Sampler.h)

set(vertex_shader_list
        shader/ColorVS.hlsl
        shader/ParticleVS.hlsl
        shader/ShadowVS.hlsl
        shader/BasePassVS.hlsl)

set(pixel_shader_list
        shader/ParticlePS.hlsl
        shader/PhongPS.hlsl
        shader/BasePassPS.hlsl)

set(shader_header_list
        shader/Common.hlsli
        shader/LightingHelper.hlsli
        shader/Particle.hlsli
        shader/BasePass.hlsli)

set_source_files_properties(${pixel_shader_list} PROPERTIES ShaderType ps ShaderModel 5_0)
set_source_files_properties(${vertex_shader_list} PROPERTIES ShaderType vs ShaderModel 5_0)

add_library(${library_name} STATIC 
        ${source_file_list} 
        ${header_file_list})

target_include_directories(${library_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(thirdparty)

target_compile_definitions(${library_name} PRIVATE 
        NOMINMAX=1 
        UNICODE=1 
        _UNICODE=1
        NENGINE_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}"
        NENGINE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        NENGINE_SHADER_BUILD_DIR="${CMAKE_BINARY_DIR}/shaders")

# TODO make tinygltf a private dependency
target_link_libraries(${library_name}
        PUBLIC tinygltf
        PUBLIC d3d11.lib
        PUBLIC dxgi.lib
        PUBLIC dxguid.lib
        PUBLIC stb_image
        PUBLIC mikktspace)

set(HLSL_SHADER_FILES)
list(APPEND HLSL_SHADER_FILES ${pixel_shader_list})
list(APPEND HLSL_SHADER_FILES ${vertex_shader_list})

add_custom_target(shaders)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

foreach(FILE ${HLSL_SHADER_FILES})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(shadertype ${FILE} ShaderType)
        get_source_file_property(shadermodel ${FILE} ShaderModel)
        add_custom_command(TARGET shaders
                        COMMAND ${FXC_EXE} /nologo 
                                /Emain 
                                /T${shadertype}_${shadermodel} 
                                $<IF:$<CONFIG:DEBUG>,/Od,/O1> 
                                /Zi 
                                /Fo ${CMAKE_BINARY_DIR}/shaders/${FILE_WE}.cso 
                                /Fd ${CMAKE_BINARY_DIR}/shaders/${FILE_WE}.pdb 
                                ${FILE}
                        MAIN_DEPENDENCY ${FILE}
                        COMMENT "HLSL ${FILE}"
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        VERBATIM)
endforeach()

add_dependencies(${library_name} shaders)
