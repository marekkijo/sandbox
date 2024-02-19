attribute vec4 vertices_buf;

uniform vec4 color;
uniform mat4 viewport;
uniform mat4 camera_rot;

varying vec4 frag_color;

void main() {
  gl_Position = viewport * camera_rot * vertices_buf;
  frag_color = color;
}
