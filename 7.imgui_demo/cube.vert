#version 410 core
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_pos;
out vec3 frag_normal;

void main()
{
    frag_pos = vec3(model * vec4(in_pos, 1.0));
    frag_normal = mat3(transpose(inverse(model))) * in_normal;
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
}
