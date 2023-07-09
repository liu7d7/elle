#pragma once

#include "usings.h"
#include "glm/detail/type_quat.hpp"
#include "object.h"
#include "lines.h"
#include "joint.h"
#include "pose.h"
#include <cpptoml.h>

#include <utility>

namespace engine {
  struct layout {
    unordered_map<string, shared_ptr<joint>> joints;
    shared_ptr<joint> root;

    explicit layout(string const& path);

    void pose_lerp(pose const& begin, pose const& end, float delta);

    void pose(pose const& p);
  };

  template<typename T>
  requires is_model_stack_provider<T> && is_mvp_provider<T>
  void draw_layout(T& m_stack, layout const& l) {
    draw_joint(m_stack, *l.root);
  }
}