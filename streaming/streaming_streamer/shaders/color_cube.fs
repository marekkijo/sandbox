#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

in vec4 frag_color;

out vec4 color;

void main() { color = frag_color; }
