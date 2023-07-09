#include "vertex_array.h"

namespace engine {
  vertex_array::vertex_array(vector<int> const& attribs) : size(0), m_vao(0), m_vbo(0), m_stride(std::accumulate(attribs.begin(),
                                                                                                                 attribs.end(), 0) *
                                                                                                 int(sizeof(float))) {
    gl_create_vertex_arrays(1, &m_vao);
    gl_create_buffers(1, &m_vbo);

    gl_vertex_array_vertex_buffer(m_vao, 0, m_vbo, 0, m_stride);
    int offset = 0;
    for (int i = 0; i < attribs.size(); i++) {
      gl_enable_vertex_array_attrib(m_vao, i);
      gl_vertex_array_attrib_format(m_vao, i, attribs[i], GL_FLOAT, false, offset * sizeof(float));
      gl_vertex_array_attrib_binding(m_vao, i, 0);
      offset += attribs[i];
    }

    COUT("vertex array " << m_vao << " created with ")
    for (auto& attrib : attribs) {
      COUT(attrib << " ")
    }
    COUT('\n')
  }
}
