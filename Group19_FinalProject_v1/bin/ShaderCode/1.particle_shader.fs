#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 FragColor;

uniform sampler2D sprite;

void main()
{
    vec4 tempColor = texture(sprite, TexCoords) * ParticleColor;
    if(tempColor.a < 0.2)
        discard;
    FragColor = tempColor;
}