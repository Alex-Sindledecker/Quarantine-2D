#version 410 core

in vec2 texCoords;

uniform sampler2D tex;

out vec4 color;

void main()
{
	color = vec4(texture(tex, vec2(texCoords.x, texCoords.y)).rgb, 1);
}