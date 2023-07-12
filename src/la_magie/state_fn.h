#pragma once

#include "state.h"

namespace la_magie {

  void resize(state&, resize_args const& args);

  void draw_main_screen(state& s);

  void draw_scene(state& s);

  void draw_hud(state& s, draw_args const& args);

  void draw_debug_lines(state& s);

  void draw_dither(state& s);

  void draw(state& s, draw_args const& args);

  void mouse_move(state& s, move_args const& args);

  void mouse_click(state& s, click_args const&);

  void key_press(state& s, key_args const& args);

  state load(application& app);
}