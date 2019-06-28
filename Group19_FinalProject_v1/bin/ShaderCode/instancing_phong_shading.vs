#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords_vs;
layout (location = 3) in mat4 instanceMatrix;

// uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} out_vs;
void main()
{
   	gl_Position = projection * view * instanceMatrix * vec4(position, 1.0f);
	out_vs.FragPos = vec3(instanceMatrix * vec4(position, 1.0f));
   	out_vs.Normal = transpose(inverse(mat3(instanceMatrix))) * normal;
   	out_vs.TexCoords = texCoords_vs;
   	out_vs.FragPosLightSpace = lightSpaceMatrix * vec4(out_vs.FragPos, 1.0f);
};