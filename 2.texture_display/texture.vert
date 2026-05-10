#version 410 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex_uv;

out vec2 out_tex_uv;

void main()
{
    out_tex_uv = in_tex_uv;
    gl_Position = vec4(in_pos, 1.0);
}
