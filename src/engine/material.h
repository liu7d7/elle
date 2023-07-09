#pragma once

#include "usings.h"
#include "utils.h"

namespace engine {
  struct material {
    vec3 ambient_ka{1.};
    vec3 diffuse_kd{1.};
    vec3 specular_ks{1.};
    float specular_exp_ns{10.};
    float opacity_d{0.};
    int illum{2};
    int ambient_map_ka{-1};
    int diffuse_map_kd{-1};
    int opacity_map_d{-1};

    [[nodiscard]] inline bool has_alpha() const {
      return opacity_d < 1. || opacity_map_d != -1;
    }
  };

  static string to_string(material const& mat) {
    std::stringstream out;
    out << "material{\n"
        << "  ambient_ka: " << mat.ambient_ka << "\n"
        << "  diffuse_kd: " << mat.diffuse_kd << "\n"
        << "  specular_ks: " << mat.specular_ks << "\n"
        << "  specular_exp_ns: " << mat.specular_exp_ns << "\n"
        << "  opacity_d: " << mat.opacity_d << "\n"
        << "  illum: " << mat.illum << "\n"
        << "  ambient_map_ka: " << mat.ambient_map_ka << "\n"
        << "  diffuse_map_kd: " << mat.diffuse_map_kd << "\n"
        << "  opacity_map_d: " << mat.opacity_map_d << "\n"
        << "}\n";
    return out.str();
  }
}