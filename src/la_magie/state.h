#pragma once

#include "engine/app.h"
#include "engine/font.h"
#include "engine/shader.h"
#include "engine/frame_buffer.h"
#include "engine/vertex_array.h"
#include "engine/camera.h"
#include "engine/layout.h"
#include "engine/pose.h"

using namespace engine;

namespace la_magie {

  struct state {

/* =======================================================defs======================================================= */

    using application = app<state>;
    application& app;

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

    explicit state(application& app) :
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

    static inline mat4 get_ortho(int width, int height) {
      return glm::ortho(0.f, float(width), float(height), 0.f, -1.f, 1.f);
    }

  };

  using application = state::application;

}