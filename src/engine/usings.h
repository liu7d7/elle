#pragma once

#include <vector>
#include <optional>
#include <memory>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "frozen/unordered_map.h"
#include "engine/string_ext.h"
#include "glad/glad.h"
#include <span>
#include <stack>
#include <chrono>
#include <future>
#include "logging.h"
#include <algorithm>
#include <cctype>
#include <functional>
#include <expected>

namespace engine {
  using uint = GLuint;
  using uchar = uint8_t;
  using ushort = uint16_t;

  using string = engine::string_ext;

  using std::tuple, std::optional, std::shared_ptr, std::unique_ptr, std::vector, std::unordered_map;
  using std::array, std::ifstream, std::byte, glm::vec2, glm::vec3, glm::vec4, glm::mat4;
  using std::span, std::tuple_size_v, std::stack, std::deque;
  using std::chrono::seconds;
  using std::future;
  using std::async;
  using std::future_status;
  using std::thread;

  using quat = glm::qua<float>;

  using std::expected;
  using std::unexpected;
}