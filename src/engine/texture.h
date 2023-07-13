#pragma once

#include "usings.h"
#include "stb/stb_image.h"

namespace engine {
  struct texture_spec {
    int internal_format;
    int format;
    int type;
    int mag_filter;
    int min_filter;
  };

  struct texture {
    uint handle{};
    int width;
    int height;
    texture_spec spec;

    texture(int width, int height, texture_spec spec);

    texture(string const& path, int min_filter, int mag_filter);

    /* implicit */ inline operator uint() const {
      return handle;
    }
  };
}
