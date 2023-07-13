#pragma once

#include "state.h"

namespace elle {

  void resize(state&, resize_args const& args);

  void draw(state& s, draw_args const& args);

  void mouse_move(state& s, move_args const& args);

  void mouse_click(state& s, click_args const&);

  void key_press(state& s, key_args const& args);

  state load(application& app);
}