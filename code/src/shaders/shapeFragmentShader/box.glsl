#version 330 core
out vec4 FragColor;

//in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform vec4 colorChange;

vec3 makeSquare(float d_Left, float d_Top, float d_Right, float d_Bottom, vec2 normalisedCoord)
{
    float left = step(d_Left, normalisedCoord.x);
    float top = step(d_Top, 1-normalisedCoord.y);
    float right = step(d_Right, 1-normalisedCoord.x);
    float bottom = step(d_Bottom, normalisedCoord.y);

    vec3 rectangle = vec3(left*top*right*bottom);
    return rectangle;
}

void main()
{
    vec2 st = TexCoord;
    vec3 color = vec3(0.);
    //Make Red "Dot" in the middle
    if(st.x > 0.45f && st.x < 0.55f && st.y > 0.45f && st.y < 0.55f)
        color = vec3(1.f,0,0);
    else
        color = makeSquare(0.1,0.1,0.1,0.1,st);
    vec4 colorFull = vec4(color,1.f);

    if(colorFull.r == 1 && colorFull.g == 1)
        colorFull.w = 0;
    
    if(colorFull.r == 0 && colorFull.g == 0)
        colorFull.xyz = colorChange.xyz;
    FragColor = colorFull;
}