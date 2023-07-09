#pragma once

#include "GLFW/glfw3.h"

namespace engine {
  enum class keybind {
    forward = GLFW_KEY_W,
    backward = GLFW_KEY_S,
    left = GLFW_KEY_A,
    right = GLFW_KEY_D,
    up = GLFW_KEY_SPACE,
    down = GLFW_KEY_LEFT_SHIFT,
    reset_zoom = GLFW_KEY_R,

  };
}