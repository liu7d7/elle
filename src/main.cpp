#include "elle/state_fn.h"

int main() {
  elle::application_t app{1280, 800, "la magie", elle::load};
  app.mouse_click += elle::mouse_click;
  app.mouse_move += elle::mouse_move;
  app.key_press += elle::key_press;
  app.draw += elle::draw;
  app.resize += elle::resize;
  app.run();
  return 0;
}
