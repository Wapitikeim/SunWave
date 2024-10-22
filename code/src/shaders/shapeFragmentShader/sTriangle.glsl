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
    
    vec2 pos = vec2(0.5)-st;

    float r = length(pos)*2.0;
    float a = atan(pos.y,pos.x);

    float f = cos(a*3.);
    // f = abs(cos(a*3.));
    // f = abs(cos(a*2.5))*.5+.3;
    // f = abs(cos(a*12.)*sin(a*3.))*.8+.1;
    // f = smoothstep(-.5,1., cos(a*10.))*0.2+0.5;

    color = vec3( 1.-smoothstep(f,f+0.02,r) );
    
    vec4 colorFull = vec4(color,1.f);

    if(colorFull.r == 0 && colorFull.g == 0)
        colorFull.w = 0;

    if(colorFull.w == 1)
        colorFull.rgb = vec3(1);

    if(colorFull.w == 1)
        colorFull.xyz = colorChange.xyz;

    FragColor = colorFull;
}