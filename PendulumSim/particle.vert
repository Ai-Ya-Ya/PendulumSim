#version 330 core
layout (location = 0) in vec2 vertex; // <vec2 position>

uniform vec2 offset;

void main()
{
    float scale = 0.003f;
    gl_Position = vec4((vertex * scale) + offset, 0.0, 1.0);
}