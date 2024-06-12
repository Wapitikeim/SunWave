//All Credits to https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/ opened 18.10.2023 18:00

#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 nearPoint;
out vec3 farPoint;

out mat4 fragView;
out mat4 fragProj;

uniform mat4 view;
uniform mat4 projection;

vec3 UnprojectPoint(float x, float y, float z, mat4 viewToInverse, mat4 projectionToInverse) {
    mat4 viewInv = inverse(viewToInverse);
    mat4 projInv = inverse(projectionToInverse);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    nearPoint = UnprojectPoint(aPos.x, aPos.y, 0.0, view, projection).xyz;
    farPoint = UnprojectPoint(aPos.x, aPos.y, 1.0, view, projection).xyz;
    
    fragView = view;
    fragProj = projection;

    gl_Position = vec4(aPos, 1.0);
}