#include "elle/state_fn.h"
#include "elle/editor.h"

int main() {
  elle::application app{1280, 800, "la magie", elle::load};
  std::jthread editor_thread{[&app](std::stop_token const& st) {
    while (!st.stop_requested()) {
      if (editor(app)) {
        break;
      }
    }
  }};
  app.mouse_click += elle::mouse_click;
  app.mouse_move += elle::mouse_move;
  app.key_press += elle::key_press;
  app.draw += elle::draw;
  app.resize += elle::resize;
  app.run();
  editor_thread.request_stop();
  return 0;
}
