#version 410 core

layout (location = 0) in vec4 pos;

uniform mat4 projection;

out vec2 uv;

void main()
{
	gl_Position = projection * vec4(pos.xy, 0.f, 1.f);
	uv = pos.zw;
}