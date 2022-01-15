#version 400 core
layout(location = 0) out vec4 FragColor;
uniform vec4 particleColor;

void main()
{
    FragColor = particleColor;
}