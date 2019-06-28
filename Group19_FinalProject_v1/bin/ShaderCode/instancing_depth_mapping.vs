#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 aColor;
layout (location = 3) in mat4 instanceMatrix;

uniform mat4 lightSpaceMatrix;
//uniform mat4 model;

out vec3 ourColor;
void main()
{
   gl_Position = lightSpaceMatrix * instanceMatrix * vec4(position, 1.0f);
   ourColor = aColor;
};