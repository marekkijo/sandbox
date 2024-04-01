#version 330 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 normal;

uniform vec4 color;
uniform mat4 viewport;
uniform mat4 camera_rot;

out vec4 frag_color;

void main() {
  gl_Position = viewport * camera_rot * vertex;

  vec4 local_normal = camera_rot * normal;
  vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0));
  float brightness = max(dot(local_normal.xyz, light_dir), 0.0);
  frag_color = color * brightness;
}
