#version 410 core

in vec2 out_tex_uv;
uniform sampler2D our_texture;

out vec4 frag_color;

void main() {
    frag_color = texture(our_texture, out_tex_uv);
}
