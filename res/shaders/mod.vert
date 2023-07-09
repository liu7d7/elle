#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tex_coord;

layout (location = 0) out vec3 v_pos;
layout (location = 1) out vec3 v_norm;
layout (location = 2) out vec2 v_tex_coord;

uniform mat4 proj;
uniform mat4 look;
uniform mat4 model;

void main() {
  vec4 fin = proj * look * model * vec4(pos, 1.0);
  gl_Position = fin;
  v_pos = fin.xyz;
  v_norm = mat3(transpose(inverse(model))) * norm;
  v_tex_coord = tex_coord;
}

