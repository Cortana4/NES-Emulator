#version 450 core

layout (location = 0) in vec2 posAttrib;
layout (location = 1) in vec2 texCoordAttrib;

out vec2 texCoord;

void main()
{
	gl_Position = vec4(posAttrib, 0.0, 1.0);
	texCoord = texCoordAttrib;
}
