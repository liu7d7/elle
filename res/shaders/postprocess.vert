#version 450

in vec2 uv;

out vec2 v_uv;

void main() {
  v_uv = uv * 0.5 + 0.5;
  gl_Position = vec4(uv, 0., 1.);
}