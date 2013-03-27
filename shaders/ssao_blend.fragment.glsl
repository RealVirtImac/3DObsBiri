#version 150
#extension GL_ARB_explicit_attrib_location : enable

uniform sampler2D ColorMap;
uniform sampler2D OcclusionMap;

in vec2 uv;

out vec4 Color;

void main(void)
{
	vec3 colors = texture2D(ColorMap,uv).xyz;
	float occlusion = texture2D(OcclusionMap,uv).x;
	
	colors = clamp(colors*occlusion,0.0,1.0);
	
	Color = vec4(vec3(pow(colors,vec3(1.0/2.2))),1.0);
}
