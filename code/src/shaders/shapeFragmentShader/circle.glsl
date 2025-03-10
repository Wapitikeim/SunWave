#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform vec4 colorChange = vec4(0,0,0,1);

float circle(in vec2 _st, in float _radius)
{
    vec2 dist = _st-vec2(0.5);
	return 1.-smoothstep(_radius-(_radius*0.01),
                         _radius+(_radius*0.01),
                         dot(dist,dist)*4.0);
}

void main()
{
    vec2 st = TexCoord;
    vec3 color = vec3(circle(st,0.5f));
    vec4 colorFull = vec4(color,1.f);

    if(colorFull.r == 0 && colorFull.g == 0)
        colorFull.w = 0;

    //Kreis initial weiß darum wechsel zu schwarz
    if(colorFull.w == 1)
        colorFull.rgb = vec3(1);

    if(colorFull.w == 1)
        colorFull.xyz = colorChange.xyz;
    
    FragColor = colorFull;
}