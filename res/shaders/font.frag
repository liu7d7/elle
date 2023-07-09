#version 460

layout (location = 0) in vec2 v_tex_coord;
layout (location = 1) in vec4 v_color;

layout (location = 0) out vec4 f_color;

uniform sampler2D tex;

void main() {
  float t = texture(tex, v_tex_coord).r;
  if (t == 0) discard;
  f_color = vec4(vec3(1), t) * v_color;
}