#pragma once

#include "usings.h"
#include "cpptoml.h"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/trigonometric.hpp"
#include "utils.h"

namespace engine {
  struct pose {
    unordered_map<string, quat> rotations;

    explicit pose(string const& s);
    pose() = default;

    [[nodiscard]] std::shared_ptr<cpptoml::table> as_toml() const;

    [[nodiscard]] inline quat at(string const& s, quat const& c = glm::identity<quat>()) const {
      auto it = rotations.find(s);
      if (it == rotations.end()) return c;
      return it->second;
    }

    [[nodiscard]] inline bool contains(string const& s) {
      return rotations.contains(s);
    }
  };

  inline pose lerp(pose const& begin, pose const& end, float delta) {
    pose p{begin};
    for (auto& [k, v] : p.rotations) {
      v = glm::slerp(v, end.at(k), delta);
    }
    for (auto& [k, v] : end.rotations) {
      if (!p.contains(k)) {
        p.rotations.emplace(k, glm::slerp(glm::identity<quat>(), v, delta));
      }
    }
    return p;
  }
}