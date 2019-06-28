#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
	vec4 FragPosLightSpace;
} gs_in[];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
	vec4 FragPosLightSpace;
} vs_out;

uniform bool isExplode;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(1.0) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {    
    vec3 normal = GetNormal();

    gl_Position = isExplode? explode(gl_in[0].gl_Position, normal) : gl_in[0].gl_Position;
    vs_out.TexCoords = gs_in[0].TexCoords;
    vs_out.FragPos = gs_in[0].FragPos;
    vs_out.Normal = gs_in[0].Normal;
	vs_out.FragPosLightSpace = gs_in[0].FragPosLightSpace;
    EmitVertex();
    gl_Position = isExplode? explode(gl_in[1].gl_Position, normal) : gl_in[1].gl_Position;
    vs_out.TexCoords = gs_in[1].TexCoords;
    vs_out.FragPos = gs_in[1].FragPos;
    vs_out.Normal = gs_in[1].Normal;
	vs_out.FragPosLightSpace = gs_in[1].FragPosLightSpace;
    EmitVertex();
    gl_Position = isExplode? explode(gl_in[2].gl_Position, normal) : gl_in[2].gl_Position;
    vs_out.TexCoords = gs_in[2].TexCoords;
    vs_out.FragPos = gs_in[2].FragPos;
    vs_out.Normal = gs_in[2].Normal;
	vs_out.FragPosLightSpace = gs_in[2].FragPosLightSpace;
    EmitVertex();
    EndPrimitive();
}