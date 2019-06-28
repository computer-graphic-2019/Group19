#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;

    vec4 FragPosLightSpace;
	vec3 Normal;
} fs_in;

struct Light {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   vec3 position;
};

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

uniform Light light;
uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
   float shadow = 0.0;
   // perform perspective divide
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   // Transform to [0,1] range
   projCoords = projCoords * 0.5 + 0.5;
   // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
   float closestDepth = texture(shadowMap, projCoords.xy).r; 
   // Get depth of current fragment from light's perspective 
   float currentDepth = projCoords.z; 
   //shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

   // Calculate bias (based on depth map resolution and slope) 
   vec3 normal = normalize(fs_in.Normal); 
   vec3 lightDir = normalize(light.position - fs_in.FragPos);
   float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);

   // PCF 
   vec2 texelSize = 0.2 / textureSize(shadowMap, 0);
   for(int x = -2; x <= 2; ++x)
   {
       for(int y = -2; y <= 2; ++y)
       {
           float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
           shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
       } 
   }
   shadow /= 25.0;

   // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
   if(projCoords.z > 1.0)
       shadow = 0.0;

   return shadow;
}

void main()
{           
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    float gamma = 2.2;
    // get diffuse color
    vec3 diffuse_color = pow(texture(diffuseMap, fs_in.TexCoords).rgb, vec3(gamma));
    // ambient
    vec3 ambient = light.ambient * diffuse_color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * diffuse_color * light.diffuse;
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
    vec3 specular = light.specular * spec;

    // simple attenuation
    float max_distance = 1.5;
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / min(distance, max_distance);
   
    diffuse *= attenuation;
    specular *= attenuation;
    
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);
    result = pow(result, vec3(1.0/gamma));

    FragColor = vec4(result, 1.0);
}