#pragma once

#include <vector>
#include <numeric>
#include <iostream>
#include "glad/glad.h"
#include "usings.h"

namespace engine {
  struct vertex_array {
    int size;

    explicit vertex_array(vector<int> const& attribs);

    template<typename C>
    void set_vertex_data(C const& vertices, uint mode = GL_DYNAMIC_DRAW) {
      size = sizeof(typename C::value_type) * vertices.size() / m_stride;
      gl_named_buffer_data(m_vbo, sizeof(typename C::value_type) * vertices.size(), data(vertices), mode);
    }

    /* implicit */ inline operator uint() const {
      return m_vao;
    }

    GLuint m_vao;
  private:
    GLuint m_vbo;
    GLint m_stride;
  };
}