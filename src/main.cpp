#include "engine/app.h"
#include "engine/font.h"
#include "engine/frame_buffer.h"
#include "engine/vertex_array.h"
#include "engine/camera.h"
#include "engine/pose.h"
#include "engine/layout.h"

using namespace engine;

static inline mat4 get_ortho(int width, int height) {
  return glm::ortho(0.f, float(width), float(height), 0.f, -1.f, 1.f);
}

struct state {

/* =======================================================defs======================================================= */

  using appli = app<state>;
  appli& app;

/* ======================================================fonts======================================================= */

  font dank_mono{"res/fonts/Dank Mono Italic.otf", 48};

/* =====================================================matrices===================================================== */

  mat4 proj{1.};
  mat4 view{1.};
  stack<mat4> model{deque<mat4>{mat4{1.}}};

/* =====================================================shaders====================================================== */

  shader dither{"res/shaders/postprocess.vert", "res/shaders/dither.frag"};
  shader debug_lines_shader{"res/shaders/debug_lines.vert", "res/shaders/debug_lines.frag"};

/* ==================================================frame buffers=================================================== */

  frame_buffer half{
    app.width / 2,
    app.height / 2, {
      {GL_COLOR_ATTACHMENT0, texture_spec{GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST}}
    }
  };

  frame_buffer intermediary{
    app.width,
    app.height, {
      {GL_COLOR_ATTACHMENT0, texture_spec{GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST}},
      {GL_DEPTH_ATTACHMENT,
       texture_spec{GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, GL_NEAREST, GL_NEAREST}}
    }
  };

/* ==================================================vertex arrays=================================================== */

  vertex_array quad{{2}};
  vertex_array debug_lines{{3, 3}};

/* =====================================================palette====================================================== */

  const vec3 dark = vec3(0.24, 0.14, 0.23);
  const vec3 light = vec3(0.96, 0.93, 0.9);

/* ======================================================other======================================================= */

  camera cam{};
  layout layout{"res/layouts/elle.layout.toml"};
  unordered_map<string, pose> poses{
    {"identity", pose{"res/poses/identity.toml"}},
    {"standing", pose{"res/poses/elle.standing.toml"}},
    {"walk_left", pose{"res/poses/elle.walk_left.toml"}},
    {"walk_right", pose{"res/poses/elle.walk_right.toml"}},
    {"start", pose{"res/poses/test.start.toml"}},
    {"finish", pose{"res/poses/test.finish.toml"}},
  };

  struct {
    string const not_editing{""};
    string current_joint{not_editing};
    string current_pose{not_editing};
    vec3 current{0};
    int ax = 0;
  } layout_editor;

  explicit state(appli& app) :
    app(app),
    proj{get_ortho(app.width, app.height)} {
    quad.set_vertex_data(vector<float>{
      -1, -1,
      -1, 1,
      1, 1,
      1, 1,
      1, -1,
      -1, -1,
    });
    debug_lines.set_vertex_data(vector<float>{
      0, 0, 0, 1, 0, 0,
      1, 0, 0, 1, 0, 0,
      0, 0, 0, 0, 1, 0,
      0, 1, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 1,
      0, 0, 1, 0, 0, 1,
    });
  }

  void push_model() {
    model.push(model.top());
  }

  void pop_model() {
    model.pop();
  }

  void update_matrices(bool three_d) {
    cam.update();
    view = cam.get_view();
    proj = three_d ? cam.get_proj(app.widthf / app.heightf) : get_ortho(app.width, app.height);
  }

  [[nodiscard]] vec3 get_view_pos() const {
    return cam.eye();
  }

  [[nodiscard]] inline mat4 get_model() const {
    return model.top();
  }

  [[nodiscard]] inline mat4 get_view() const {
    return view;
  }

  [[nodiscard]] inline mat4 get_proj() const {
    return proj;
  }

  [[nodiscard]] inline mat4& model_ref() {
    return model.top();
  }

};

using appli = state::appli;

void resize(state& s, resize_args const& args) {
  gl_viewport(0, 0, args.width, args.height);
  s.proj = get_ortho(args.width, args.height);
}

void draw_main_screen(state& s) {
  s.update_matrices(false);

  s.dank_mono.draw_centered(s, "elle", {s.app.widthf / 2.f, s.app.heightf * 0.4f}, {vec4(0., 0., 0., 1.), 4});
  s.dank_mono.draw_centered(s, "play", {s.app.widthf * 0.35f, s.app.heightf * 0.65f},
                            {vec4(0., 0., 0., 1.), 1.75f + (sinf(glfw_get_time()) + 1.f) / 16.f});
  s.dank_mono.draw_centered(s, "quit", {s.app.widthf * 0.65f, s.app.heightf * 0.65f},
                            {vec4(0., 0., 0., 1.), 1.75f + (sinf(glfw_get_time()) + 1.f) / 16.f});
}

float last_press = 0.f;

void draw_scene(state& s) {
  s.update_matrices(true);

  float time = glfw_get_time() * 2.66f;
  static string last_foot = "";
  static vec3 last_pos{0.f};
  static vec3 offset{0.f};
  string foot = int((time - half_pi) / pi) % 2 ? "right_calf" : "left_calf";
  vec3 pos = s.layout.at(foot)->final_pos;
  if (foot == last_foot) {
    offset += pos - last_pos;
  }
  last_foot = std::move(foot);
  last_pos = pos;
  s.layout.pose_lerp(s.poses.at("walk_left"), s.poses.at("walk_right"), glm::clamp((sinf(time) + 1) * 0.5f, 0.f, 1.f));
  s.push_model();
  s.model_ref() = glm::translate(s.model_ref(), offset);
  draw_layout(s, s.layout);
  s.pop_model();
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
  gl_viewport(0, 0, s.app.width / 2, s.app.height / 2);
  gl_cull_face(GL_FRONT);
  gl_bind_framebuffer(GL_FRAMEBUFFER, s.half);
  gl_clear(GL_COLOR_BUFFER_BIT);
  gl_use_program(s.dither);
  gl_active_texture(GL_TEXTURE0);
  gl_bind_texture(GL_TEXTURE_2D, s.intermediary.attachments.at(GL_COLOR_ATTACHMENT0));
  s.dither.set("tex", 0);
  s.dither.set("dark", s.dark);
  s.dither.set("light", s.light);
  gl_bind_vertex_array(s.quad);
  gl_draw_arrays(GL_TRIANGLES, 0, 6);
  gl_viewport(0, 0, s.app.width, s.app.height);
  gl_cull_face(GL_BACK);
}

void draw(state& s, draw_args const& args) {
  for (int it : {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT}) {
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
  gl_blit_named_framebuffer(s.half, 0, 0, 0, s.app.width / 2, s.app.height / 2, 0, 0, s.app.width, s.app.height,
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
  } else if (args.key == GLFW_KEY_R) {
    last_press = glfw_get_time();
  }
}

state load(appli& app) {
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

int main() {
  appli app{1280, 800, "elle", load};
  std::jthread t([&app](std::stop_token const& st) {
    while (!st.stop_requested()) {
      if (app.state.has_value()) {
        auto& s = app.state.value();
        string editing;
        std::getline(std::cin, editing);
        editing = trim(editing);
        auto write_pose = [](const string& name, const pose& pose) {
          std::ofstream ofs(name);
          ofs << *pose.as_toml();
          COUT(*pose.as_toml() << std::endl)
          ofs.flush();
          ofs.close();
        };

        if (editing.starts_with("a") && s.layout.joints.contains(s.layout_editor.current_joint) &&
            s.poses.contains(s.layout_editor.current_pose)) {
          std::istringstream iss(editing);
          string _;
          iss >> _;
          vec3 v;
          iss >> v.x >> v.y >> v.z;
          constexpr auto u = vec3{0, 1, 0};
          quat q = u_to_v(u, v);
          s.poses.at(s.layout_editor.current_pose).rotations[s.layout_editor.current_joint] = q;
          s.layout.pose(s.poses.at(s.layout_editor.current_pose));
          COUT(glm::axis(q) << " " << glm::degrees(glm::angle(q)) << " " << u << " " << v << std::endl)
        } else if (editing.starts_with("save") && s.poses.contains(s.layout_editor.current_pose)) {
          string name = trim(editing.substr(4));
          write_pose(name, s.poses.at(s.layout_editor.current_pose));
        } else if (editing.starts_with("impactful")) {
          string i, joint;
          std::cin >> i >> joint;
          float x, y, z, ang;
          std::cin >> x >> y >> z >> ang;
          quat q = glm::angleAxis(glm::radians(ang), vec3{x, y, z});
          stack<shared_ptr<struct joint>> joints;
          joints.push(s.layout.at(joint));
          while (!joints.empty()) {
            
          }
        } else if (editing.starts_with("quit")) {
          glfw_set_window_should_close(app, true);
          break;
        } else if (editing.starts_with("edit")) {
          editing = trim(editing.substr(4));
          if (s.poses.contains(editing)) {
            s.layout_editor.current_pose = editing;
            s.layout.pose(s.poses.at(editing));
            COUT("editing " << editing << std::endl)
          } else {
            COUT("not a joint" << std::endl)
            s.layout_editor.current_pose.clear();
          }
        } else if (editing.starts_with("joint")) {
          editing = trim(editing.substr(5));
          if (s.layout.joints.contains(editing)) {
            s.layout_editor.current_joint = editing;
            COUT("editing joint " << editing << std::endl)
          } else {
            COUT("not a joint" << std::endl)
            s.layout_editor.current_joint.clear();
          }
        }
      }
    }
  });
  app.draw += draw;
  app.resize += resize;
  app.mouse_move += mouse_move;
  app.mouse_click += mouse_click;
  app.key_press += key_press;
  app.run();
  return 0;
}
