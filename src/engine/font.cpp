#include "font.h"

namespace engine {
  struct font_state font_state{};

  font::font(string const& path, float height) : height(height), page{}, ascent{}, chars{} {
    auto buffer = std::make_unique<uchar[]>(1 << 24);

    {
      FILE* file = fopen(path.c_str(), "rb");
      fseek(file, 0, SEEK_END);
      int len = ftell(file);

      if (len == -1) {
        fclose(file);
        throw std::runtime_error("failed to read file");
      }

      fseek(file, 0, SEEK_SET);

      fread(buffer.get(), 1, len, file);
      fclose(file);
    }

    stbtt_fontinfo font_info;
    if (!stbtt_InitFont(&font_info, buffer.get(), 0)) throw std::runtime_error("failed to init font");

    stbtt_pack_context pack_ctx;
    auto bitmap = std::make_unique<uchar[]>(tex_size * tex_size);
    if (!stbtt_PackBegin(&pack_ctx, bitmap.get(), tex_size, tex_size, 0, 1, nullptr))
      throw std::runtime_error("failed to init font");
    stbtt_PackSetOversampling(&pack_ctx, 4, 4);
    stbtt_PackFontRange(&pack_ctx, buffer.get(), 0, height, 32, 256, chars.data());
    stbtt_PackEnd(&pack_ctx);

    {
      int asc;
      stbtt_GetFontVMetrics(&font_info, &asc, nullptr, nullptr);
      ascent = float(asc) * stbtt_ScaleForMappingEmToPixels(&font_info, height);
    }

    gl_gen_textures(1, &page);
    gl_bind_texture(GL_TEXTURE_2D, page);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl_tex_image_2d(GL_TEXTURE_2D, 0, GL_RED, tex_size, tex_size, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.get());
  }

  float font::width_of(std::string const& str, float scale) const {
    float w = 0;
    for (char c : str) {
      if (c == '&') continue;
      w += chars[c].xadvance * scale * 0.86f;
    }
    return w;
  }

  stbtt_packedchar font::get_char(char c) const {
    if (c < 32 || c >= 32 + num_chars) c = ' ';
    return chars[c - 32];
  }

  float font::height_of(float scale) const {
    return height * scale;
  }
}