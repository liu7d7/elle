#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

layout (location = 0) out vec3 v_color;

uniform mat4 proj;
uniform mat4 view;

void main() {
  gl_Position = proj * view * vec4(pos, 1.0);
  v_color = color;
}