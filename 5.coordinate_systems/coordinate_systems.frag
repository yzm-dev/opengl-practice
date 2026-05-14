#version 410 core

in vec2 out_uv;

out vec4 frag_color;

uniform sampler2D texture_1;
uniform sampler2D texture_2;

void main()
{
    // lerp计算混合两张纹理的颜色值，0.2表示texture_1的权重为0.2，texture_2的权重为0.8
    frag_color = mix(texture(texture_1, out_uv), texture(texture_2, out_uv), 0.2);
}
