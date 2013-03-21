#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec3 Position;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(Position,1.0);
}
