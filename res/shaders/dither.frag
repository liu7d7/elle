#version 460
in vec2 v_uv;

out vec4 color;

uniform sampler2D tex;
uniform vec4 palette[256];
uniform int palette_size;

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

vec3 hsl_to_rgb(vec3 c) {
  vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);

  return c.z + c.y * (rgb - 0.5) * (1.0 - abs(2.0 * c.z - 1.0));
}

vec3 rgb_to_hsl(in vec3 c) {
  float h = 0.0;
  float s = 0.0;
  float l = 0.0;
  float r = c.r;
  float g = c.g;
  float b = c.b;
  float cMin = min(r, min(g, b));
  float cMax = max(r, max(g, b));

  l = (cMax + cMin) / 2.0;
  if (cMax > cMin) {
    float cDelta = cMax - cMin;

    s = l < .0 ? cDelta / (cMax + cMin) : cDelta / (2.0 - (cMax + cMin));

    if (r == cMax) {
      h = (g - b) / cDelta;
    } else if (g == cMax) {
      h = 2.0 + (b - r) / cDelta;
    } else {
      h = 4.0 + (r - g) / cDelta;
    }

    if (h < 0.0) {
      h += 6.0;
    }
    h = h / 6.0;
  }
  return vec3(h, s, l);
}

vec3 rgb_to_hsv(vec3 c) {
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
  vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float dist(float a, float b) {
  return abs(a - b);
}

vec4[2] closest_colors(float lightness) {
  vec4 ret[2];
  vec4 closest = vec4(0, 0, 0, -50);
  vec4 second_closest = vec4(0, 0, 0, -50);
  for (int i = 0; i < palette_size; ++i) {
    float temp_distance = dist(palette[i].w, lightness);
    if (temp_distance < dist(closest.w, lightness)) {
      second_closest = closest;
      closest = palette[i];
    } else {
      if (temp_distance < dist(second_closest.w, lightness)) {
        second_closest = palette[i];
      }
    }
  }
  ret[0] = closest;
  ret[1] = second_closest;
  return ret;
}

vec3 dither(vec3 col) {
  vec3 hsl = rgb_to_hsl(col);
  vec4 colors[2] = closest_colors(hsl.z);
  float lightness_diff = dist(hsl.z, colors[0].w) / dist(colors[1].w, colors[0].w);
  return lightness_diff <= idx_val() ? vec3(colors[0]) : vec3(colors[1]);
}

void main() {
  color = vec4(dither(texture(tex, v_uv).rgb), 1.);
}