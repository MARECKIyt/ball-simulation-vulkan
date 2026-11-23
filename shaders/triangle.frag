#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uv;
layout(location = 0) out vec4 outColor;

void main()
{
    float d = length(uv);
    float aa = fwidth(d);
    float alpha = 1.0 - smoothstep(1.0 - aa, 1.0, d);
    if (alpha <= 0.0) discard;
    outColor = vec4(fragColor, alpha);
}