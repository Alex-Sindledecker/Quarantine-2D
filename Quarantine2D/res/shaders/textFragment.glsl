#version 410 core

in vec2 uv;

uniform sampler2D tex;
uniform vec4 color;

out vec4 fragColor;

void main()
{
	fragColor = color * texture(tex, uv).r;
}