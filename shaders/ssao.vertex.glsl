#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec2 Position;
layout (location = 2) in vec2 UV;

out vec2 uv;

void main(void)
{
	gl_Position = vec4(Position.xy, 0.0, 1.0);
        uv = Position * 0.5 + 0.5;
}
