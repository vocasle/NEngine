#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 tex_coords;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D tex_sampler;

layout(binding = 2) uniform uniform_buffer_object_ps {
    vec3 light_pos;
	vec3 cam_pos;
} ubo_ps;


void main() {
//    out_color = vec4(texture(tex_sampler, tex_coords).rgb * frag_color, 1.0);
	out_color = vec4(normal, 1.0);
}