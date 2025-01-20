#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform vec4 colorChange = vec4(0,0,0,1);

vec3 makeSquare(float d_Left, float d_Top, float d_Right, float d_Bottom, vec2 normalisedCoord)
{
    float left = step(d_Left, normalisedCoord.x);
    float top = step(d_Top, 1.0 - normalisedCoord.y);
    float right = step(d_Right, 1.0 - normalisedCoord.x);
    float bottom = step(d_Bottom, normalisedCoord.y);

    vec3 rectangle = vec3(left * top * right * bottom);
    return rectangle;
}

void main()
{
    vec2 st = TexCoord;
    vec3 color = vec3(0.0);

    // Define the outer square
    vec3 outerSquare = makeSquare(0.1, 0.1, 0.1, 0.1, st);

    // Define the inner square
    vec3 innerSquare = makeSquare(0.3, 0.3, 0.3, 0.3, st);

    // Combine the squares
    color = outerSquare;

    vec4 colorFull = vec4(color, 1.0);

    // Make the area between the inner and outer squares transparent
    if (innerSquare.r == 0.0 && outerSquare.r == 1.0)
        colorFull.w = 0.0;

    // Make the inside of the inner square fully opaque
    if (innerSquare.r == 1.0)
        colorFull.w = 1.0;

    // Apply the color change
    if (colorFull.w == 1.0)
        colorFull.rgb = colorChange.rgb;

    FragColor = colorFull;
}