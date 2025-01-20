#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform vec4 colorChange = vec4(0,0,0,1);

float line(vec2 p, vec2 a, vec2 b, float thickness)
{
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return smoothstep(thickness, thickness + 0.01, length(pa - ba * h));
}

void main()
{
    vec2 st = TexCoord;

    // Define the vertices of the triangle
    vec2 v0 = vec2(0.5, 0.95);
    vec2 v1 = vec2(0.05, 0.05);
    vec2 v2 = vec2(0.95, 0.05);

    // Define the thickness of the lines
    float thickness = 0.03;

    // Calculate the distance to each edge
    float d0 = line(st, v0, v1, thickness);
    float d1 = line(st, v1, v2, thickness);
    float d2 = line(st, v2, v0, thickness);

    // Combine the distances to get the final shape
    float shape = min(min(d0, d1), d2);

    // Set the color for the lines
    vec4 colorFull = vec4(colorChange.rgb, 1.0);

    // Make the outside of the triangle transparent
    if (shape > 0.5)
        colorFull.w = 0.0;

    FragColor = colorFull;
}