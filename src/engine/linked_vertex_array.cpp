#include <numeric>
#include "linked_vertex_array.h"

namespace engine {
  linked_vertex_array::linked_vertex_array(vector<int> const& attribs) : m_vao(0), m_vbo(0), m_ibo(0), size(0),
                                                                         m_stride(std::accumulate(attribs.begin(),
                                                                                                  attribs.end(), 0) * int(sizeof(float))) {
    gl_create_vertex_arrays(1, &m_vao);
    gl_create_buffers(1, &m_vbo);
    gl_create_buffers(1, &m_ibo);

    gl_vertex_array_vertex_buffer(m_vao, 0, m_vbo, 0, m_stride);
    gl_vertex_array_element_buffer(m_vao, m_ibo);

    int offset = 0;
    for (int i = 0; i < attribs.size(); i++) {
      gl_enable_vertex_array_attrib(m_vao, i);
      gl_vertex_array_attrib_format(m_vao, i, attribs[i], GL_FLOAT, false, offset * sizeof(float));
      gl_vertex_array_attrib_binding(m_vao, i, 0);
      offset += attribs[i];
    }
  }
}
