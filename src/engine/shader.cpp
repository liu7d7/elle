#include <iostream>
#include "shader.h"

namespace engine {
  uint shader::create_shader(int type, string const& path, char const* src) {
    if (shader::precompiled.contains(path)) {
      return shader::precompiled[path];
    }
    uint handle = gl_create_shader(type);
    int successful;
    gl_shader_source(handle, 1, &src, nullptr);
    gl_compile_shader(handle);
    gl_get_shaderiv(handle, GL_COMPILE_STATUS, &successful);
    if (successful != GL_TRUE) {
      char buf[512];
      gl_get_shader_info_log(handle, 512, nullptr, buf);
      throw std::runtime_error(path + ": " + buf);
    }
    return shader::precompiled[path] = handle;
  }

  void link(uint handle) {
    int successful;
    gl_link_program(handle);
    gl_get_programiv(handle, GL_LINK_STATUS, &successful);
    if (successful != GL_TRUE) {
      char buf[512];
      gl_get_program_info_log(handle, 512, nullptr, buf);
      throw std::runtime_error(std::to_string(handle) + ": " + buf);
    }
  }

  string type_to_string(uint type) {
    switch (type) {
      case GL_FLOAT:
        return "float";
      case GL_FLOAT_VEC2:
        return "vec2";
      case GL_FLOAT_VEC3:
        return "vec3";
      case GL_FLOAT_VEC4:
        return "vec4";
      case GL_DOUBLE:
        return "double";
      case GL_DOUBLE_VEC2:
        return "dvec2";
      case GL_DOUBLE_VEC3:
        return "dvec3";
      case GL_DOUBLE_VEC4:
        return "dvec4";
      case GL_INT:
        return "int";
      case GL_INT_VEC2:
        return "ivec2";
      case GL_INT_VEC3:
        return "ivec3";
      case GL_INT_VEC4:
        return "ivec4";
      case GL_UNSIGNED_INT:
        return "uint";
      case GL_UNSIGNED_INT_VEC2:
        return "uvec2";
      case GL_UNSIGNED_INT_VEC3:
        return "uvec3";
      case GL_UNSIGNED_INT_VEC4:
        return "uvec4";
      case GL_BOOL:
        return "bool";
      case GL_BOOL_VEC2:
        return "bvec2";
      case GL_BOOL_VEC3:
        return "bvec3";
      case GL_BOOL_VEC4:
        return "bvec4";
      case GL_FLOAT_MAT2:
        return "mat2";
      case GL_FLOAT_MAT3:
        return "mat3";
      case GL_FLOAT_MAT4:
        return "mat4";
      case GL_SAMPLER_2D_ARRAY:
        return "sampler2DArray";
    }
    return "unknown";
  }

  shader::shader(string const& vsh, string const& fsh, string const& gsh /* = "" */) :
    m_handle(gl_create_program()), m_uniforms{} {

    string vsp = read(vsh);
    string fsp = read(fsh);
    bool has_gsh = !gsh.empty();

    uint vs, fs, gs;
    vs = create_shader(GL_VERTEX_SHADER, vsh, vsp.c_str());
    fs = create_shader(GL_FRAGMENT_SHADER, fsh, fsp.c_str());

    if (has_gsh) {
      string gsp = read(gsh);
      gs = create_shader(GL_GEOMETRY_SHADER, gsh, gsp.c_str());
    }

    gl_attach_shader(m_handle, vs);
    gl_attach_shader(m_handle, fs);
    if (has_gsh) {
      gl_attach_shader(m_handle, gs);
    }

    link(m_handle);

    int uniform_count;
    gl_get_programiv(m_handle, GL_ACTIVE_UNIFORMS, &uniform_count);
    COUT("uniform count: " << uniform_count << '\n')
    for (int i = 0; i < uniform_count; i++) {
      char name[64];
      uint type;
      int a, b;
      gl_get_active_uniform(m_handle, i, 64, &a, &b, &type, name);
      int loc = gl_get_uniform_location(m_handle, name);
      m_uniforms.emplace(string(name), uniform(loc));
      COUT("uniform " << name << " at " << std::dec << loc << " of type " << type_to_string(type) << std::hex << " (" << type << ')'
                      << " added to shader " << m_handle << '\n')
    }
  }


  unordered_map<string, uint> shader::precompiled;
}
