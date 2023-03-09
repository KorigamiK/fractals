#version 330 core

in vec2 a_position;

out vec2 v_texcoord;

void main() {
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_texcoord = (a_position + vec2(1.0)) / vec2(2.0);
}
