#pragma once

#include <memory>
#include "usings.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/detail/type_quat.hpp"
#include "object.h"

namespace engine {
  struct joint : public std::enable_shared_from_this<joint> {
    string name;
    quat rot{glm::identity<quat>()};
    mat4 fin_rot{glm::identity<mat4>()};
    float offset{};
    vec3 offset_3d{0, 1, 0};
    bool dark{};
    float width{0.03f};
    bool visible{};
    optional<tuple<object, vec3, vec3, quat>> obj;
    vector<shared_ptr<joint>> children;
    shared_ptr<joint> parent{nullptr};

    explicit joint(string name);

    inline void link(shared_ptr<joint> const& child) {
      children.push_back(child);
      child->parent = shared_from_this();
    }
  };

  template<typename T>
  requires is_model_stack_provider<T> && is_mvp_provider<T>
  void draw_joint(T& m_stack, joint const& j, int depth = 0) {
    mat4 orig = m_stack.model_ref();

    if (j.obj.has_value()) {
      auto const& [obj, pos, scale, rot] = j.obj.value();
      m_stack.push_model();
      m_stack.model_ref() = m_stack.model_ref() * glm::scale(glm::identity<mat4>(), scale);
      draw_obj(m_stack, obj);
      m_stack.pop_model();
    }

    for (auto const& c : j.children) {
      m_stack.push_model();

      m_stack.model_ref() = orig * glm::rotate(glm::identity<mat4>(), glm::angle(c->rot), glm::axis(c->rot));
      c->fin_rot = clear_translation(m_stack.model_ref());
      c->offset_3d = vec3{clear_translation(m_stack.model_ref()) * vec4{0, c->offset, 0, 1}};

      if (c->visible) {
        draw_line(m_stack, vec3{0., 0., 0.}, vec3{0, c->offset, 0}, c->width, vec3{0.});
      }

      m_stack.model_ref() = glm::translate(glm::identity<mat4>(), c->offset_3d) * m_stack.model_ref();

      draw_joint(m_stack, *c, depth + 1);

      m_stack.pop_model();
    }
  }
}