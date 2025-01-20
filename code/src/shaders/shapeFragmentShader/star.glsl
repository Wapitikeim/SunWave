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

    // Define the vertices of the star
    vec2 v0 = vec2(0.5, 0.95);
    vec2 v1 = vec2(0.6, 0.6);
    vec2 v2 = vec2(0.95, 0.6);
    vec2 v3 = vec2(0.65, 0.4);
    vec2 v4 = vec2(0.8, 0.05);
    vec2 v5 = vec2(0.5, 0.3);
    vec2 v6 = vec2(0.2, 0.05);
    vec2 v7 = vec2(0.35, 0.4);
    vec2 v8 = vec2(0.05, 0.6);
    vec2 v9 = vec2(0.4, 0.6);

    // Define the thickness of the lines
    float thickness = 0.03;

    // Calculate the distance to each edge of the star
    float d0 = line(st, v0, v1, thickness);
    float d1 = line(st, v1, v2, thickness);
    float d2 = line(st, v2, v3, thickness);
    float d3 = line(st, v3, v4, thickness);
    float d4 = line(st, v4, v5, thickness);
    float d5 = line(st, v5, v6, thickness);
    float d6 = line(st, v6, v7, thickness);
    float d7 = line(st, v7, v8, thickness);
    float d8 = line(st, v8, v9, thickness);
    float d9 = line(st, v9, v0, thickness);

    // Combine the distances to get the final shape
    float shape = min(min(min(min(min(min(min(min(min(d0, d1), d2), d3), d4), d5), d6), d7), d8), d9);

    // Set the color for the lines
    vec4 colorFull = vec4(colorChange.rgb, 1.0);

    // Make the outside of the star transparent
    if (shape > 0.5)
        colorFull.w = 0.0;

    FragColor = colorFull;
}