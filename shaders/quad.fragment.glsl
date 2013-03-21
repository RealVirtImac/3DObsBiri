#version 150
#extension GL_ARB_explicit_attrib_location : enable

in vec2 UV;

out vec3 color;

uniform sampler2D renderedTexture1;
uniform sampler2D renderedTexture2;

void main()
{
	vec3 one_texture = texture(renderedTexture1,UV).xyz;
	vec3 one = vec3(1.0, one_texture.g, one_texture.b);
	
	vec3 two_texture = texture(renderedTexture2,UV).xyz;
	vec3 two = vec3(two_texture.r, 1.0, 1.0);
	
	color = vec3(one.rgb * two.rgb);
}
