#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform vec4 colorChange = vec4(0,0,0,1);

float box(in vec2 _st, in vec2 _size)
{
    _size = vec2(0.5) - _size*0.5;
    vec2 uv = smoothstep(_size,
                        _size+vec2(0.001),
                        _st);
    uv *= smoothstep(_size,
                    _size+vec2(0.001),
                    vec2(1.0)-_st);
    return uv.x*uv.y;
}

float cross(in vec2 _st, float _size)
{
    return  box(_st, vec2(_size,_size/6.)) +
            box(_st, vec2(_size/6.,_size));
}

void main()
{
    vec2 st = TexCoord;
    vec3 color = vec3(cross(st, 1.f));
    vec4 colorFull = vec4(color,1.f);

    //Ecken am Kreuz
    if(colorFull.r == 0 && colorFull.g == 0)
        colorFull.w = 0;

    //Kreuz initial weiß darum wechsel zu schwarz
    if(colorFull.w == 1)
        colorFull.rgb = vec3(1);

    if(colorFull.w == 1)
        colorFull.xyz = colorChange.xyz;

    FragColor = colorFull;
}