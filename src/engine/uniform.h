#pragma once

#include <variant>
#include <stdexcept>
#include <iostream>
#include "usings.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "frozen/unordered_map.h"
#include "material.h"

namespace engine {
  template<typename T>
  concept is_uniform =
  std::same_as<std::remove_reference_t<T>, int> ||
  std::same_as<std::remove_reference_t<T>, uint> ||
  std::same_as<std::remove_reference_t<T>, float> ||
  std::same_as<std::remove_reference_t<T>, vec2> ||
  std::same_as<std::remove_reference_t<T>, vec3> ||
  std::same_as<std::remove_reference_t<T>, vec4> ||
  std::same_as<std::remove_reference_t<T>, mat4>;

  struct uniform {
    explicit uniform(int location);

    inline void set(int val) const {
      gl_uniform_1i(m_loc, val);
    }

    inline void set(uint val) const {
      gl_uniform_1i(m_loc, int(val));
    }

    inline void set(float val) const {
      gl_uniform_1f(m_loc, val);
    }

    inline void set(vec2 val) const {
      gl_uniform_2f(m_loc, val.x, val.y);
    }

    inline void set(vec3 val) const {
      gl_uniform_3f(m_loc, val.x, val.y, val.z);
    }

    inline void set(vec4 val) const {
      gl_uniform_4f(m_loc, val.x, val.y, val.z, val.w);
    }

    inline void set(mat4 val) const {
      gl_uniform_matrix_4fv(m_loc, 1, false, &val[0][0]);
    }

  private:
    int m_loc;

    friend struct std::hash<uniform>;
  };
}

template<>
struct std::hash<engine::uniform> {
  bool operator ()(engine::uniform const& t) const {
    return t.m_loc;
  }
};