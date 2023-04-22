#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_tex_coords;
layout(location = 3) in vec3 in_normal;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 tex_coords;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 frag_world_pos;

layout(binding = 0) uniform uniform_buffer_object {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
    frag_color = in_color;
    tex_coords = in_tex_coords;
    normal = (ubo.model * vec4(in_normal, 0.0)).xyz;
    frag_world_pos = (ubo.model * vec4(in_position, 1.0)).xyz;
}