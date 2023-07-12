#include "la_magie/state_fn.h"
#include "la_magie/editor.h"

using namespace la_magie;

int main() {
  application app{1280, 800, "la magie", load};
  std::jthread editor_thread{[&app](std::stop_token const& st) {
    while (!st.stop_requested()) {
      if (editor(app)) {
        break;
      }
    }
  }};
  app.mouse_click += mouse_click;
  app.mouse_move += mouse_move;
  app.key_press += key_press;
  app.draw += draw;
  app.resize += resize;
  app.run();
  editor_thread.request_stop();
  return 0;
}
