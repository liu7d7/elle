#pragma once

#include "entity.h"
#include "elle/state.h"

namespace elle {
  struct elle : public entity {
    layout layout{"res/poses/elle.layout.toml"};
    float yaw{0};

    void draw(state& s) override {
      s.push_model();
      s.model_ref() = glm::translate(s.model_ref(), pos);
      s.model_ref() = glm::rotate(s.model_ref(), glm::radians(yaw), vec3{0, 1, 0});
      draw_layout(s, s.elle);
      s.pop_model();
    }

    void quick_update(state& s) override {
      float time = float(glfw_get_time()) * 4.f;
      static string last_foot = "";
      static vec3 last_pos{0.f};
      s.elle.pose_lerp(s.poses.at("walk_left"), s.poses.at("walk_right"),
                       glm::clamp((sinf(time) + 1) * 0.5f, 0.f, 1.f));
      string foot = int((time - half_pi) / pi) % 2 ? "left_calf" : "right_calf";
      vec3 pos = s.elle.at(foot)->final_pos;
      if (foot == last_foot) {
        this->pos -= pos - last_pos;
      }
      last_foot = std::move(foot);
      last_pos = pos;
    }

    void position_camera(camera& c) {
      c.pitch = -75;
      c.yaw = 0;
      c.third_person = true;
      c.third_person_dist = 10;
    }
  };
}