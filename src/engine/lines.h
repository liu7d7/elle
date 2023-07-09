#pragma once

#include "object.h"
#include "lazy.h"
#include "GLFW/glfw3.h"

namespace engine {
  struct lines_state {
    lazy<object, string> cyl{"res/models/line_midres.obj"};
  };

  extern struct lines_state lines_state;

  template<typename T>
  requires is_model_stack_provider<T> && is_mvp_provider<T>
  void draw_line(T& m_stack, vec3 a, vec3 b, float width, vec3 color) {
    vec3 avg = (a + b) * 0.5f;
    vec3 t = normalize(b - a);

    mat4 rotation = change_axis(t, 1);

    float len = length(b - a);
    m_stack.push_model();
    m_stack.model_ref() = m_stack.model_ref() * rotation;
    m_stack.model_ref() = glm::scale(m_stack.model_ref(), vec3{width, len, width});
    m_stack.model_ref() = glm::translate(m_stack.model_ref(), a);
    draw_obj(m_stack, *lines_state.cyl);
    m_stack.pop_model();
  }
}