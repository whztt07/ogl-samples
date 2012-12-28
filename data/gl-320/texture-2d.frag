#version 150 core

uniform sampler2D Diffuse;

in block
{
	vec2 Texcoord;
} In;

out vec4 FragColor;

void main()
{
	FragColor = texture(Diffuse, In.Texcoord);
}