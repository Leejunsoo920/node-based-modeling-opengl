#version 330 core
out vec4 FragColor;

in vec2 TexCoords;


uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = vec4(vec3(0.0,1.0,0.0),1.0f);
}