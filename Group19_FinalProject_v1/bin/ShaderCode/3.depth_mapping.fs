#version 330 core

in VS_OUT {
	vec3 ourColor;
} vs_in;

out vec4 FragColor;
void main()
{
  	//gl_FragDepth = gl_FragCoord.z;
  	//FragColor = vec4(ourColor, 1.0f);
};