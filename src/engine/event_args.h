#pragma once

#include "usings.h"
#include "event.h"

namespace engine {
  struct move_args {
    float x, y;
    float dx, dy;
  };

  struct click_args {
    float x, y;
    int button, mods;
  };

  struct scroll_args {
    float x, y;
    float x_off, y_off;
  };

  struct key_args {
    int key, scancode, mods;
  };

  struct char_args {
    uint codepoint;
  };

  struct draw_args {
    double delta_time;
  };

  struct update_args {
    int age;
    const float delta_time = 1.f / 20.f;
  };

  struct resize_args {
    int width, height;
  };

  struct no_args {
  };
}