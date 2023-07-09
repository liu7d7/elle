#pragma once

#include "usings.h"
#include "texture.h"

namespace engine {
  struct frame_buffer {
    uint handle;
    unordered_map<uint, texture> attachments;
    vector<tuple<uint, texture_spec>> attachment_specs;
    int width, height;

    frame_buffer(int width, int height, vector<tuple<uint, texture_spec>> const& attachments);

    void resize(int t_width, int t_height);

    /* implicit */ inline operator uint() {
      return handle;
    }

  private:
    void attach_attachments();
  };
}