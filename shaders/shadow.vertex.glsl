#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec2 uv;
out vec3 normal;
out vec3 position;

void main(void)
{	
	uv = UV;
	normal = vec3(modelMatrix * vec4(Normal, 1.0));; 
	position = vec3(modelMatrix * vec4(Position, 1.0));
	gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
}
