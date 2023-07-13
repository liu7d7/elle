#include "utils.h"
#include <glm/gtc/quaternion.hpp>

namespace engine {
  glm::quat rotation;
  glm::vec3 translation;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::vec3 scale;

  quat extract_rotation(mat4 mat) {
    glm::decompose(mat, scale, rotation, translation, skew, perspective);
    return rotation;
  }
}