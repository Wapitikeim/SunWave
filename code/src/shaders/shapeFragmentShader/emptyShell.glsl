#version 330 core
out vec4 FragColor;

//in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform vec4 colorChange = vec4(0,0,0,1);

void main()
{
    vec2 st = TexCoord;
    vec3 color = vec3(0.);
    

    FragColor = colorFull;
}