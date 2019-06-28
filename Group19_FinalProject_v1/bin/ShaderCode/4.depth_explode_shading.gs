#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec3 ourColor;
} gs_in[];

out VS_OUT {
	vec3 ourColor;
} vs_out;

uniform bool isExplode;
uniform mat4 lightSpaceMatrix;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(2.0) + 1.0) / 2.0) * magnitude; 
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
	
	vec4 pos = isExplode? explode(gl_in[0].gl_Position, normal) : gl_in[0].gl_Position;
    gl_Position = lightSpaceMatrix * pos;
    vs_out.ourColor = gs_in[0].ourColor;
    EmitVertex();
	pos = isExplode? explode(gl_in[1].gl_Position, normal) : gl_in[1].gl_Position;
    gl_Position = lightSpaceMatrix * pos;
    vs_out.ourColor = gs_in[1].ourColor;
    EmitVertex();
	pos = isExplode? explode(gl_in[2].gl_Position, normal) : gl_in[2].gl_Position;
    gl_Position = lightSpaceMatrix * pos;
    vs_out.ourColor = gs_in[2].ourColor;
    EmitVertex();
    EndPrimitive();
}