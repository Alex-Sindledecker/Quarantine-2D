#version 410 core

layout (location = 0) in vec2 pos;

uniform mat4 projection;
uniform mat4 model;
uniform vec4 texRect;

out vec2 uv;

void main()
{
	gl_Position = projection * model * vec4(pos, 0.f, 1.f);
	if (pos.x == 1 && pos.y == 0)
	{
		uv = vec2(texRect.x + texRect.z, texRect.y);
	}
	else if (pos.x == 0 && pos.y == 0)
	{
		uv = vec2(texRect.x, texRect.y);
	}
	else if (pos.x == 0 && pos.y == 1)
	{
		uv = vec2(texRect.x, texRect.y + texRect.w);
	}
	else
	{
		uv = vec2(texRect.x + texRect.z, texRect.y + texRect.w);
	}
}