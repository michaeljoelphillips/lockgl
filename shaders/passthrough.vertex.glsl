#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

void main(void)
{
	gl_Position = vec4(vertexPosition_modelspace, 1);
}
