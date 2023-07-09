#include "texture.h"
#include "stb/stb_image.h"
#include "logging.h"
#include "filesystem"

unordered_map<string, tuple<int, int, uint>> precompiled;

namespace engine {
  texture::texture(int width, int height, texture_spec spec) :
      width(width), height(height), spec(spec) {
    gl_gen_textures(1, &handle);
    gl_bind_texture(GL_TEXTURE_2D, handle);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, spec.mag_filter);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, spec.min_filter);
    gl_tex_image_2d(GL_TEXTURE_2D, 0, spec.internal_format, width, height, 0, spec.format, spec.type, nullptr);
  }

  texture::texture(string const& path, int min_filter, int mag_filter) :
    handle(0), height(0), width(0), spec{GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, mag_filter, min_filter} {
    if (precompiled.contains(path)) {
      auto [t_width, t_height, t_handle] = precompiled[path];
      width = t_width;
      height = t_height;
      handle = t_handle;
      return;
    }
    int n_channels;
    if (!std::filesystem::exists(std::filesystem::path(path.c_str()))) {
      throw std::runtime_error(path + " does not exist!");
    }
    uchar* pixels = stbi_load(path.c_str(), &width, &height, &n_channels, 4);
    gl_gen_textures(1, &handle);
    gl_bind_texture(GL_TEXTURE_2D, handle);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    gl_tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

    gl_tex_image_2d(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    stbi_image_free(pixels);
    COUT("created texture " << std::dec << handle << '\n')
    precompiled[path] = {width, height, handle};
  }
}