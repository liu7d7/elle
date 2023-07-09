#pragma once

#include "usings.h"
#include "uniform.h"

namespace engine {
  struct shader {
    shader(string const& vsh, string const& fsh, string const& gsh = "");

    void array(string const& id, int size) {
      for (int i = 1; i < size; i++) {
        string name = id + "[" + std::to_string(i) + "]";
        int loc = gl_get_uniform_location(m_handle, name.c_str());
        m_uniforms.emplace(name, uniform{loc});
        COUT("uniform " << name << " at " << std::dec << loc << " added to shader " << m_handle << '\n')
      }
    }

    template<typename T>
    requires is_uniform<T>
    void set(string const& id, T val) {
      if (!m_uniforms.contains(id)) {
        throw std::runtime_error("uniform " + id + " not found");
      }
      m_uniforms.at(id).set(val);
    }

    /* implicit */ inline operator uint() const {
      return m_handle;
    }

  private:
    unordered_map<string, uniform> m_uniforms;
    uint m_handle;
    static unordered_map<string, uint> precompiled;

    static uint create_shader(int type, string const& path, char const* src);
  };
}