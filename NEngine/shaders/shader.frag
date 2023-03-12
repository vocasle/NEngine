#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 tex_coords;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D tex_sampler;


void main() {
    out_color = texture(tex_sampler, tex_coords);
}