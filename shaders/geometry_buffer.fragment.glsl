#version 150
#extension GL_ARB_explicit_attrib_location : enable

in vec2 uv;
in vec3 position;
in vec3 normal;

uniform sampler2D diffuse_texture;

out vec4 out_color;
out vec4 out_normal;
out vec4 out_position;

void main(void)
{
	vec3 diffuse = texture(diffuse_texture, uv).rgb;
	out_color = vec4(diffuse, 1.0);
	out_normal = vec4(normal, 1.0);
	out_position = vec4(position,1.0);
}
