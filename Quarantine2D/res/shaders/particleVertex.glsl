#version 410 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 particleColor;
layout (location = 2) in mat4 model;

uniform mat4 projection;

out vec4 color;

void main()
{
	gl_Position = projection * model * vec4(pos, 0.f, 1.f);
	color = particleColor;
}