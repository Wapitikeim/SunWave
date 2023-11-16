#version 330 core
out vec4 FragColor;

uniform vec4 changeColor;

void main()
{
    FragColor = changeColor;
} 