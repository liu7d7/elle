#pragma once

#include "usings.h"

namespace engine {
  struct linked_vertex_array {
    int size;

    explicit linked_vertex_array(vector<int> const& attribs);

    template<typename C>
    void set_vertex_data(C const& vertices, uint mode = GL_DYNAMIC_DRAW) {
      gl_named_buffer_data(m_vbo, sizeof(typename C::value_type) * vertices.size(), data(vertices), mode);
    }

    template<typename C>
    requires std::is_integral_v<typename C::value_type> && std::is_unsigned_v<typename C::value_type>
    void set_index_data(C const& indices, uint mode = GL_DYNAMIC_DRAW) {
      size = indices.size();
      gl_named_buffer_data(m_ibo, indices.size() * sizeof(typename C::value_type), data(indices), mode);
    }

    /* implicit */ inline operator uint() const {
      return m_vao;
    }

  private:
    uint m_vao;
    uint m_vbo;
    uint m_ibo;
    int m_stride;
  };
}