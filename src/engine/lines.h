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
  void draw_line(T& s, vec3 a, vec3 b, float width, vec3 color) {
    auto& [i, j, k, l] = lines_state.cyl->groups.at(0);
    l.diffuse = color;

    vec3 t = normalize(b - a);

    mat4 rotation = change_axis(t, 1);

    float len = length(b - a);
    s.push_model();
    s.model_ref() = s.model_ref() * rotation;
    s.model_ref() = glm::scale(s.model_ref(), vec3{width, len, width});
    s.model_ref() = glm::translate(s.model_ref(), a);
    draw_obj(s, *lines_state.cyl);
    s.pop_model();
  }
}