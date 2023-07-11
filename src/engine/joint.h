#pragma once

#include <memory>
#include "usings.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/detail/type_quat.hpp"
#include "object.h"

namespace engine {
  struct joint : public std::enable_shared_from_this<joint> {
    string name;
    optional<tuple<object, vec3, vec3, quat>> obj;
    vector<shared_ptr<joint>> children;
    shared_ptr<joint> parent{nullptr};
    quat rot{glm::identity<quat>()};
    vec3 final_pos{0.};
    float offset{};
    float width{0.03f};
    bool dark{};
    bool visible{};

    explicit joint(string name);

    inline void link(shared_ptr<joint> const& child) {
      children.push_back(child);
      child->parent = shared_from_this();
    }
  };

  template<typename T>
  requires is_model_stack_provider<T> && is_mvp_provider<T>
  void draw_joint(T& m_stack, joint const& j, int depth = 0) {
    m_stack.push_model();
    vec3 offset = j.rot * vec3{0, j.offset, 0};

    if (j.visible && j.parent) {
      draw_line(m_stack, {0, 0, 0}, offset, j.width, vec3{0});
    }

    m_stack.model_ref() = glm::translate(m_stack.model_ref(), offset);

    if (j.obj.has_value()) {
      auto const& [obj, pos, scale, rot] = j.obj.value();
      m_stack.push_model();
      m_stack.model_ref() = m_stack.model_ref() * glm::scale(mat4{1.}, scale) * glm::rotate(mat4{1.}, glm::angle(j.rot), glm::axis(j.rot));
      draw_obj(m_stack, obj);
      m_stack.pop_model();
    }

    for (auto const& c : j.children) {
      c->final_pos = j.final_pos + offset;
      draw_joint(m_stack, *c, depth + 1);
    }

    m_stack.pop_model();
  }
}