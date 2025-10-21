layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;

uniform mat4 mvp;

out vec4 frag_color;

void main() {
  gl_Position = mvp * pos;
  frag_color = color;
}
