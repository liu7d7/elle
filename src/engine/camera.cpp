#include "camera.h"
#include "keybind.h"

namespace engine {
  void camera::scroll(float y_off) {
    speed += y_off;
  }

  void camera::update() {
    front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    front.y = sinf(glm::radians(pitch));
    front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    right = normalize(cross(front, world_up));
    up = normalize(cross(right, front));
  }

  void camera::move(float x_off, float y_off, bool constrain_pitch) {
    x_off *= sensitivity;
    y_off *= sensitivity;

    yaw += x_off;
    pitch += y_off;

    if (constrain_pitch) {
      pitch = glm::clamp(pitch, -89.f, 89.f);
    }

    update();
  }

  void camera::key(keybind direction, float delta_time) {
    float velocity = speed * delta_time;
    if (direction == keybind::forward)
      velo += normalize(front * glm::vec3(1, 0, 1)) * velocity;
    if (direction == keybind::backward)
      velo -= normalize(front * glm::vec3(1, 0, 1)) * velocity;
    if (direction == keybind::left)
      velo -= right * velocity;
    if (direction == keybind::right)
      velo += right * velocity;
    if (direction == keybind::up)
      velo += up * velocity;
    if (direction == keybind::down)
      velo -= up * velocity;
    pos += velo;
    velo = vec3(0.);
  }

  camera::camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
      front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(default_speed), sensitivity(default_sensitivity), zoom(default_zoom),
      pos(position), world_up(up), yaw(yaw), pitch(pitch), velo(0.f) {
    update();
  }

  camera::camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) :
      camera(vec3{pos_x, pos_y, pos_z}, vec3{up_x, up_y, up_z}, yaw, pitch) {}
}