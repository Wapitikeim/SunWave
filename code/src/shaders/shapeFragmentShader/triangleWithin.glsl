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

    // Define the vertices of the outer triangle
    vec2 v0_outer = vec2(0.5, 0.95);
    vec2 v1_outer = vec2(0.05, 0.05);
    vec2 v2_outer = vec2(0.95, 0.05);

    // Define the vertices of the inner triangle
    vec2 v0_inner = vec2(0.5, 0.6);
    vec2 v1_inner = vec2(0.3, 0.2);
    vec2 v2_inner = vec2(0.7, 0.2);

    // Define the thickness of the lines
    float thickness = 0.03;

    // Calculate the distance to each edge of the outer triangle
    float d0_outer = line(st, v0_outer, v1_outer, thickness);
    float d1_outer = line(st, v1_outer, v2_outer, thickness);
    float d2_outer = line(st, v2_outer, v0_outer, thickness);

    // Calculate the distance to each edge of the inner triangle
    float d0_inner = line(st, v0_inner, v1_inner, thickness);
    float d1_inner = line(st, v1_inner, v2_inner, thickness);
    float d2_inner = line(st, v2_inner, v0_inner, thickness);

    // Combine the distances to get the final shape
    float shape_outer = min(min(d0_outer, d1_outer), d2_outer);
    float shape_inner = min(min(d0_inner, d1_inner), d2_inner);

    // Set the color for the lines
    vec4 colorFull = vec4(colorChange.rgb, 1.0);

    // Make the outside of the outer triangle and inside of the inner triangle transparent
    if (shape_outer > 0.5 || shape_inner < 0.5)
        colorFull.w = 0.0;

    // Ensure the inner triangle lines are visible
    if (shape_inner < 0.5)
        colorFull = vec4(colorChange.rgb, 1.0);

    FragColor = colorFull;
}