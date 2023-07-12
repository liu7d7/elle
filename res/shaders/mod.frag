#version 460

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_norm;
layout (location = 2) in vec2 v_tex_coord;

layout (location = 0) out vec4 f_color;

uniform vec3 view_pos;

const vec3 light_dir = vec3(1.);

uniform sampler2D map_ka;
uniform sampler2D map_kd;
uniform sampler2D map_ks;

uniform bool has_map_ka;
uniform bool has_map_kd;
uniform bool has_map_ks;

uniform float shininess;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

vec3 get_ka() {
  if (has_map_ka) {
    return texture(map_ka, v_tex_coord).rgb * ka.rgb;
  } else {
    return ka.rgb;
  }
}

vec3 get_kd() {
  if (has_map_kd) {
    return texture(map_kd, v_tex_coord).rgb * kd.rgb;
  } else {
    return kd.rgb;
  }
}

vec3 get_ks() {
  if (has_map_ks) {
    return texture(map_ks, v_tex_coord).rgb * ks.rgb;
  } else {
    return ks.rgb;
  }
}

vec3 phong() {
  // ambient
  vec3 ambient = get_ka();

  // diffuse
  vec3 norm = normalize(v_norm);
  vec3 light_dir = normalize(light_dir);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffuse = diff * get_kd();

  // specular
  vec3 view_dir = normalize(view_pos - v_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
  vec3 specular = spec * get_ks();
  if (diff < 0.001) {
    specular = vec3(0.0);
  }

  vec3 result = (ambient + diffuse + specular);
  return result;
}

void main() {
  f_color = vec4(phong(), 1.);
}