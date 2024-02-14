attribute vec4 vertex_buf;
attribute vec4 color_buf;

uniform mat4   viewport;
uniform mat4   camera_rot;

varying vec4   frag_color;

void main() {
  gl_Position   = viewport * camera_rot * vertex_buf;
  frag_color = color_buf;
}
