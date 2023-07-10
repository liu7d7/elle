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
  };
}