#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 instPosition;
layout(location = 2) in vec2 instSizeAspect;
layout(location = 3) in vec3 instColor;

layout(location = 0) out vec3 fragColor;

void main()
{
    float size = instSizeAspect.x;
    float aspect = instSizeAspect.y;

    vec2 p = position * size + instPosition;
    float simWidthInNDC = 2.0 / aspect;
    p.x = p.x / aspect;
    p.x += -1.0 + simWidthInNDC / 2.0;

    gl_Position = vec4(p, 0.0, 1.0);
    fragColor = instColor;
}