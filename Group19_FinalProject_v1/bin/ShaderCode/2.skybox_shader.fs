#version 330 core

in vec3 TexCoords;
out vec4 color;
 
uniform float time;
uniform samplerCube skybox;
 
void main()
{
	float threshold = 0.25;
	float fade = time > 0 ? time + threshold : threshold;
    color = fade * texture(skybox, TexCoords);
}