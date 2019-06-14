#version 330 core
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 lightDirection;
uniform vec3 viewPos;

void main()
{    
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.8);

    // ambient
    vec3 ambient = 0.5 * color;

    // diffuse
    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = 0.5 * diff * color;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    vec3 specular = 0.5 * spec * color;
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}