#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 tex_coords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 frag_world_pos;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D tex_sampler;

layout(binding = 2) uniform uniform_buffer_object {
    vec3 light_pos;
	vec3 cam_pos;
} ubo;


void main() {

	vec3 n = normalize(normal);
	vec3 v = normalize(ubo.cam_pos - frag_world_pos);
	vec3 l = normalize(ubo.light_pos - frag_world_pos);
	vec3 h = normalize(v + l);

    vec4 color = vec4(texture(tex_sampler, tex_coords).rgb * frag_color, 1.0);

	float kD = max(dot(n, l), 0.0);
	vec3 diffuse = kD * color.rgb;

	float kS = pow(max(dot(n, h), 0.0), 32.0);
	vec3 specular = kS * color.rgb;

	out_color = vec4(diffuse + specular, 1.0);
}