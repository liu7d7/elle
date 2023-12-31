#include "state_fn.h"

namespace elle {

  void resize(state& s, resize_args const& args) {
    gl_viewport(0, 0, args.width, args.height);
    s.intermediary.resize(args.width, args.height);

    float ratio = float(args.width) / float(args.height);
    COUT(std::dec << 400 * ratio << ' ' << 400 << '\n')
    s.half.resize(args.width / 2, args.height / 2);
  }

  void draw_dot(state& s, vec3 const& vec) {
    s.push_model();
    s.model_ref() = glm::translate(s.model_ref(), vec);
    s.model_ref() = glm::scale(s.model_ref(), vec3{0.05});
    draw_obj(s, s.dot);
    s.pop_model();
  }

  void draw_main_screen(state& s) {
    s.update_matrices(false);

    s.dank_mono.draw_centered(s, "la magie", {s.app.widthf / 2.f, s.app.heightf * 0.4f}, {vec4(0., 0., 0., 1.), 4});
    s.dank_mono.draw_centered(s, "play", {s.app.widthf * 0.35f, s.app.heightf * 0.65f},
                              {vec4(0., 0., 0., 1.), 1.75f + (float(sin(glfw_get_time())) + 1.f) / 16.f});
    s.dank_mono.draw_centered(s, "quit", {s.app.widthf * 0.65f, s.app.heightf * 0.65f},
                              {vec4(0., 0., 0., 1.), 1.75f + (float(sin(glfw_get_time())) + 1.f) / 16.f});
  }

  void draw_scene(state& s) {
    s.update_matrices(true);


  }

  void draw_hud(state& s, draw_args const& args) {
    gl_depth_func(GL_ALWAYS);
    s.update_matrices(false);

    s.dank_mono.draw(s, "fps: " + std::to_string(1. / double(args.delta_time)), {10, 10}, {vec4(s.dark, 1.), 0.5});
    gl_depth_func(GL_LEQUAL);
  }

  void draw_debug_lines(state& s) {
    s.update_matrices(true);

    gl_depth_func(GL_ALWAYS);
    gl_bind_framebuffer(GL_FRAMEBUFFER, 0);
    gl_use_program(s.debug_lines_shader);
    s.debug_lines_shader.set("proj", s.proj);
    s.debug_lines_shader.set("view", s.view);
    gl_bind_vertex_array(s.debug_lines);
    gl_draw_arrays(GL_LINES, 0, 6);
    gl_depth_func(GL_LEQUAL);
  }

  void draw_dither(state& s) {
    gl_viewport(0, 0, s.half.width, s.half.height);
    gl_cull_face(GL_FRONT);
    gl_bind_framebuffer(GL_FRAMEBUFFER, s.half);
    gl_clear(GL_COLOR_BUFFER_BIT);
    gl_use_program(s.dither);
    gl_active_texture(GL_TEXTURE0);
    gl_bind_texture(GL_TEXTURE_2D, s.intermediary.attachments.at(GL_COLOR_ATTACHMENT0));
    s.dither.set("tex", 0);
    gl_bind_vertex_array(s.quad);
    gl_draw_arrays(GL_TRIANGLES, 0, 6);
    gl_viewport(0, 0, s.intermediary.width, s.intermediary.height);
    gl_cull_face(GL_BACK);
  }

  void draw(state& s, draw_args const& args) {
    for (int it: {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT}) {
      if (glfw_get_key(s.app, it) == GLFW_PRESS) {
        s.cam.key(keybind(it), args.delta_time);
      }
    }

    s.cam.update();

    gl_bind_framebuffer(GL_FRAMEBUFFER, s.intermediary);
    gl_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_scene(s);

    draw_dither(s);

    gl_bind_framebuffer(GL_FRAMEBUFFER, 0);
    gl_blit_named_framebuffer(s.half, 0, 0, 0, s.half.width, s.half.height, 0, 0, s.app.width, s.app.height,
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);

    draw_debug_lines(s);
    draw_hud(s, args);
  }

  void mouse_move(state& s, move_args const& args) {
    if (s.app.mouse_state() == GLFW_CURSOR_DISABLED)
      s.cam.move(args.dx, -args.dy);
  }

  void mouse_click(state& s, click_args const&) {
    s.app.mouse_state(GLFW_CURSOR_DISABLED);
  }

  void key_press(state& s, key_args const& args) {
    if (args.key == GLFW_KEY_ESCAPE) {
      s.app.mouse_state(GLFW_CURSOR_NORMAL);
    }
  }

  state load(application_t& app) {
    COUT(app.widthf << ' ' << app.heightf << '\n')
    gl_enable(GL_BLEND);
    gl_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gl_enable(GL_DEPTH_TEST);
    gl_depth_func(GL_LEQUAL);

    gl_enable(GL_CULL_FACE);
    gl_cull_face(GL_FRONT);

    gl_clear_color(1, 1, 1, 1);

    gl_viewport(0, 0, app.width, app.height);

    app.mouse_state(GLFW_CURSOR_DISABLED);

    return state(app);
  }
}