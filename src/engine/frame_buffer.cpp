#include "frame_buffer.h"

namespace engine {
  auto init_attachments(vector<tuple<uint, texture_spec>> const& attachments, int width, int height) {
    unordered_map<uint, texture> res;
    res.reserve(attachments.size());
    for (auto [attachment, texture_spec] : attachments) {
      res.emplace(attachment, texture{
          width,
          height,
          texture_spec
      });
    }
    return res;
  }

  frame_buffer::frame_buffer(int width, int height, vector<tuple<uint, texture_spec>> const& attachments) :
    width(width), height(height), attachment_specs(attachments), attachments(init_attachments(attachments, width, height)) {
    gl_create_framebuffers(1, &handle);
    attach_attachments();
  }

  void frame_buffer::resize(int t_width, int t_height) {
    width = t_width;
    height = t_height;
    for (auto [uint, texture] : attachments) {
      gl_delete_textures(1, &texture.handle);
    }
    attachments = init_attachments(attachment_specs, t_width, t_height);
    attach_attachments();
  }

  void frame_buffer::attach_attachments() {
    for (auto [attachment, texture] : this->attachments) {
      gl_named_framebuffer_texture(handle, attachment, texture, 0);
    }
    gl_bind_framebuffer(GL_FRAMEBUFFER, handle);
    if (gl_check_named_framebuffer_status(handle, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw std::runtime_error("frame buffer is not complete");
    }
  }
}