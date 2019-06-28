#version 330 core
in VS_OUT {
   vec3 FragPos;
   vec3 Normal;
   vec2 TexCoords;
   vec4 FragPosLightSpace;
} in_fs;
struct Light {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   vec3 position;
   };
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadowMap;
uniform vec3 viewPos;
uniform Light light;
out vec4 FragColor;
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
   vec3 normal = normalize(in_fs.Normal); 
   vec3 lightDir = normalize(light.position - in_fs.FragPos);
   float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);

   // PCF 
   vec2 texelSize = 0.2 / textureSize(shadowMap, 0);
   for(int x = -7; x <= 7; ++x)
   {
       for(int y = -7; y <= 7; ++y)
       {
           float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
           shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
       } 
   }
   shadow /= 225.0;

   // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
   if(projCoords.z > 1.0)
       shadow = 0.0;

   return shadow;
}

void main()
{
   float gamma = 2.2;
   //vec3 diffuse_color = texture(texture_diffuse1, in_fs.TexCoords).rgb;
   vec3 diffuse_color = pow(texture(texture_diffuse1, in_fs.TexCoords).rgb, vec3(gamma));
   vec3 specular_color = texture(texture_specular1, in_fs.TexCoords).rgb;
   //ambient
   vec3 ambient = light.ambient;
   //diffuse
   vec3 normal = normalize(in_fs.Normal);
   vec3 lightDir = normalize(light.position - in_fs.FragPos);
   float diff = max(dot(normal, lightDir), 0.0f);
   vec3 diffuse = diff * light.diffuse;
   //specular
   vec3 viewDir = normalize(viewPos - in_fs.FragPos);
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
   vec3 specular = spec * light.specular;
   // simple attenuation
   float max_distance = 1.5;
   float distance = length(light.position - in_fs.FragPos);
   float attenuation = 1.0 / min(distance, max_distance);
   
   diffuse *= attenuation;
   specular *= attenuation;
   // add shadow 
   float shadow = ShadowCalculation(in_fs.FragPosLightSpace);
   vec3 result = ambient * diffuse_color + (1.0 - shadow) * (diffuse * diffuse_color + specular * specular_color);
   result = pow(result, vec3(1.0/gamma));
   FragColor = vec4(result, 1.0f);
};