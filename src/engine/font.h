#pragma once

#include <array>
#include <fstream>
#include "stb/stb_truetype.h"
#include "usings.h"
#include "shader.h"
#include "linked_vertex_array.h"
#include "lazy.h"

namespace engine {
  struct font_state {
    lazy<shader, string, string> sh{"res/shaders/font.vert", "res/shaders/font.frag"};
    lazy<linked_vertex_array, vector<int>> lva{{2, 2, 4}};
    unordered_map<char, vec4(*)(vec4 color, vec4 orig_color)> formats{
      {'r', [](vec4 color, vec4 orig_color) { return orig_color; }}
    };
  };

  extern struct font_state font_state;

  struct font_style {
    vec4 color;
    float scale;
  };

  struct font {
    static constexpr int tex_size = 2048;
    static constexpr float tex_sizef = float(tex_size);
    static constexpr int num_chars = 256;

    float height;
    uint page;
    float ascent;
    std::array<stbtt_packedchar, num_chars> chars;

    font(string const& path, float height);

    [[nodiscard]] float width_of(std::string const& str, float scale) const;

    [[nodiscard]] stbtt_packedchar get_char(char c) const;

    [[nodiscard]] float height_of(float scale) const;

    template<typename T>
    requires is_proj_provider<T>
    void draw(T const& proj, std::string const& str, vec2 pos, font_style style) {
      std::vector<float> vertices;
      constexpr int vertex_size = 2 + 2 + 4;
      vertices.reserve(vertex_size * 4 * str.size());

      std::vector<uint> indices(6 * str.size());
      for (int i = 0; i < str.size(); i++) {
        indices[i * 6 + 0] = i * 4 + 0;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;
        indices[i * 6 + 3] = i * 4 + 2;
        indices[i * 6 + 4] = i * 4 + 3;
        indices[i * 6 + 5] = i * 4 + 0;
      }

      int len = (int) str.size();
      vec2 d = pos;
      d.y += ascent * style.scale;
      vec4 orig_color = style.color;
      vec4 color = orig_color;
      for (int i = 0; i < len; i++) {
        char c = str[i];
        char p = i > 0 ? str[i - 1] : 0;
        if (p == '&') continue;

        if (c == '&' && i < len - 1) {
          char next = std::tolower(str[i + 1]);

          if (font_state.formats.contains(next)) {
            auto f = font_state.formats.at(next);
            color = f(color, orig_color);
            continue;
          }
        }

        auto pc = get_char(c);
        if (c != ' ') {
          float dxs = d.x + pc.xoff * style.scale;
          float dys = d.y + pc.yoff * style.scale;

          float dx1s = d.x + pc.xoff2 * style.scale;
          float dy1s = d.y + pc.yoff2 * style.scale;

          std::array<float, 4 * vertex_size> verts{
            dxs, dys, (float) pc.x0 / tex_sizef, (float) pc.y0 / tex_sizef, color.x, color.y, color.z, color.w,
            dxs, dy1s, (float) pc.x0 / tex_sizef, (float) pc.y1 / tex_sizef, color.x, color.y, color.z, color.w,
            dx1s, dy1s, (float) pc.x1 / tex_sizef, (float) pc.y1 / tex_sizef, color.x, color.y, color.z, color.w,
            dx1s, dys, (float) pc.x1 / tex_sizef, (float) pc.y0 / tex_sizef, color.x, color.y, color.z, color.w,
          };

          vertices.insert(vertices.end(), verts.begin(), verts.end());
        }

        d.x += pc.xadvance * style.scale * 0.86f;
      }

      font_state.lva->set_vertex_data(vertices);
      font_state.lva->set_index_data(indices);

      gl_bind_vertex_array(*font_state.lva);
      gl_use_program(*font_state.sh);
      font_state.sh->set("proj", proj.get_proj());
      gl_active_texture(GL_TEXTURE0);
      gl_bind_texture(GL_TEXTURE_2D, page);
      font_state.sh->set("tex", 0);
      gl_draw_elements(GL_TRIANGLES, font_state.lva->size, GL_UNSIGNED_INT, nullptr);
    }

    template<typename T>
    requires is_proj_provider<T>
    void draw_centered_h(T const& proj, std::string const& str, vec2 pos, font_style style) {
      draw(proj, str, pos - vec2{width_of(str, style.scale) / 2.f, 0}, style);
    }

    template<typename T>
    requires is_proj_provider<T>
    void draw_centered_v(T const& proj, std::string const& str, vec2 pos, font_style style) {
      draw(proj, str, pos - vec2{0, height_of(style.scale) / 2.f}, style);
    }

    template<typename T>
    requires is_proj_provider<T>
    void draw_centered(T const& proj, std::string const& str, vec2 pos, font_style style) {
      draw(proj, str, pos - vec2{width_of(str, style.scale) / 2.f, height_of(style.scale) / 2.f}, style);
    }
  };
}