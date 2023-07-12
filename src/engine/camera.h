#pragma once

#include "usings.h"
#include "glm/ext/matrix_transform.hpp"
#include "GLFW/glfw3.h"
#include "keybind.h"
#include "glm/ext/matrix_clip_space.hpp"

namespace engine {
  const float default_yaw = -90.0f;
  const float default_pitch = 0.0f;
  const float default_speed = 3.25f;
  const float default_sensitivity = 0.025f;
  const float default_zoom = 45.0f;
  const vec3 default_pos = {0., 0., 0.};
  const vec3 default_up = {0., 1., 0.};

  struct camera {
    vec3 pos;
    vec3 velo;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float zoom;

    explicit camera(vec3 position = default_pos, vec3 up = default_up, float yaw = default_yaw,
                    float pitch = default_pitch);

    camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

    [[nodiscard]] inline vec3 eye() const {
      return pos;
    }

    [[nodiscard]] inline mat4 get_view() const {
      return glm::lookAt(eye(), pos + front, up);
    }

    [[nodiscard]] inline mat4 get_proj(float aspect_ratio) const {
      return glm::perspective(zoom, aspect_ratio, 0.01f, 1000.f);
    }

    void key(keybind direction, double delta_time);

    void move(float x_off, float y_off, bool constrain_pitch = true);

    void scroll(float y_off);

    void update();
  };
}
