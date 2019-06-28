#version 330 core
layout (location = 0) in vec3 position;
layout (location = 0) in vec2 texCoords;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

void main()
{
    TexCoords = texCoords;
    ParticleColor = color;
    gl_Position = projection * view * model * vec4(position * 0.2, 1.0f);
}