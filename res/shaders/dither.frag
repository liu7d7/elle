#version 460
in vec2 v_uv;

out vec4 color;

uniform sampler2D tex;
uniform vec3 light;
uniform vec3 dark;

const float index_matrix_4x4[16] = float[](
0., 8., 2., 10.,
12., 4., 14., 6.,
3., 11., 1., 9.,
15., 7., 13., 5.
);

//const float index_matrix_8x8[64] = float[](
//0.,  32., 8.,  40., 2.,  34., 10., 42.,
//48., 16., 56., 24., 50., 18., 58., 26.,
//12., 44., 4.,  36., 14., 46., 6.,  38.,
//60., 28., 52., 20., 62., 30., 54., 22.,
//3.,  35., 11., 43., 1.,  33., 9.,  41.,
//51., 19., 59., 27., 49., 17., 57., 25.,
//15., 47., 7.,  39., 13., 45., 5.,  37.,
//63., 31., 55., 23., 61., 29., 53., 21.
//);

float idx_val() {
  int x = int(mod(gl_FragCoord.x, 4));
  int y = int(mod(gl_FragCoord.y, 4));
  return index_matrix_4x4[x + y * 4] / 16.;
}

vec3 to_hsl(vec3 c) {
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
  vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 to_rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 dither(vec3 col) {
  vec3 hsl = to_hsl(col);
  return hsl.z > idx_val() ? light : dark;
}

void main() {
  color = vec4(dither(texture(tex, v_uv).rgb), 1.);
}