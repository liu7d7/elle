#version 460

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec4 color;

layout (location = 0) out vec2 v_tex_coord;
layout (location = 1) out vec4 v_color;

uniform mat4 proj;

void main() {
  gl_Position = proj * vec4(pos, 0, 1);
  v_tex_coord = tex_coord;
  v_color = color;
}