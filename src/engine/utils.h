#pragma once

#include <fstream>
#include <sstream>
#include <filesystem>
#include <glm/gtx/matrix_decompose.hpp>
#include "usings.h"

namespace engine {
  constexpr float half_pi = glm::half_pi<float>();
  constexpr float pi = glm::pi<float>();

  static string read(string const& path) {
    string out;
    ifstream stream(path);
    if (!stream.is_open()) {
      throw std::runtime_error(path + " does not exist!");
    }
    return (std::stringstream() << stream.rdbuf()).str();
  }

  template<typename T>
  static auto indexed(T&& container) {
    using std::begin, std::end;
    return tuple{begin(container), end(container), 0};
  }

  template<int W, int H, typename T>
  static std::ostream& operator<<(std::ostream& out, glm::mat<W, H, T> mat) {
    for (int i = 0; i < H; i++) {
      for (int j = 0; j < W; j++) {
        out << std::setw(8) << std::fixed << std::setprecision(3) << mat[j][i] << ' ';
      }
      out << '\n';
    }
    return out;
  }

  template<int W, typename T>
  static std::ostream& operator<<(std::ostream& out, glm::vec<W, T> vec) {
    for (int j = 0; j < W; j++) {
      out << std::setw(8) << std::fixed << std::setprecision(3) << vec[j] << ' ';
    }
    return out;
  }

  template<int W, typename T>
  static std::string to_string(glm::vec<W, T> vec) {
    std::stringstream ss;
    for (int j = 0; j < W; j++) {
      ss << std::setw(8) << std::fixed << std::setprecision(3) << vec[j] << ' ';
    }
    return ss.str();
  }

  static std::ostream& operator<<(std::ostream& out, quat q) {
    for (int j = 0; j < 4; j++) {
      out << std::setw(8) << std::fixed << std::setprecision(3) << q[j] << ' ';
    }
    return out;
  }

  inline vec3 extract_translation(mat4 const& mat) {
    return {mat[3][0], mat[3][1], mat[3][2]};
  }

  quat extract_rotation(mat4 mat);

  inline float length_squared(const vec3& u) {
    return u.x * u.x + u.y * u.y + u.z * u.z;
  }

  inline quat u_to_v(vec3 u, vec3 v) {
    u = normalize(u), v = normalize(v);
    vec3 a = normalize(cross(u, v));
    quat q;
    q.x = a.x;
    q.y = a.y;
    q.z = a.z;
    q.w = sqrt(glm::dot(u, u) * glm::dot(v, v)) + glm::dot(u, v);
    return normalize(q);
  }

  inline mat4 clear_translation(mat4 mat) {
    mat[3][0] = 0;
    mat[3][1] = 0;
    mat[3][2] = 0;
    return mat;
  }

  static vec4 with_alpha(vec4 orig, float alpha) {
    return {orig.r, orig.g, orig.b, alpha};
  }

  static inline std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
      return !std::isspace(ch);
    }));
    return s;
  }

  static inline std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
      return !std::isspace(ch);
    }).base(), s.end());
    return s;
  }

  static inline std::string trim(std::string const& s) {
    return ltrim(rtrim(s));
  }

  // I wish UFCS was a thing!!!
  template<typename K, typename V>
  static inline V at_or(unordered_map<K, V> const& map, K const& k, V const& defval) {
    auto it = map.find(k);
    if (it == map.end()) return defval;
    return it->second;
  }

  static mat4 change_axis(vec3 axis, int ax_num) {
    vec3 t = normalize(axis);
    int min = 0;
    if (abs(t[min]) > abs(t[1])) min = 1;
    if (abs(t[min]) > abs(t[2])) min = 2;

    vec3 m{0.f};
    m[min] = 1;

    vec3 f = normalize(cross(t, m));
    vec3 s = normalize(cross(t, f));

    switch (ax_num) {
      case 0:
        return {
          t.x, t.y, t.z, 0,
          f.x, f.y, f.z, 0,
          s.x, s.y, s.z, 0,
          0, 0, 0, 1
        };
      case 1:
        return {
          s.x, s.y, s.z, 0,
          t.x, t.y, t.z, 0,
          f.x, f.y, f.z, 0,
          0, 0, 0, 1
        };
      case 2:
        return {
          f.x, f.y, f.z, 0,
          s.x, s.y, s.z, 0,
          t.x, t.y, t.z, 0,
          0, 0, 0, 1
        };
      default:
        throw std::runtime_error("Invalid axis number: " + std::to_string(ax_num));
    }
  }

  template<typename T>
  concept is_proj_provider = requires(T const t) {
    { t.get_proj() } -> std::convertible_to<mat4>;
  };

  template<typename T>
  concept is_view_provider = requires(T const t) {
    { t.get_view() } -> std::convertible_to<mat4>;
  };

  template<typename T>
  concept is_model_provider = requires(T const t) {
    { t.get_model() } -> std::convertible_to<mat4>;
  };

  template<typename T>
  concept is_mvp_provider = is_proj_provider<T> && is_view_provider<T> && is_model_provider<T>;

  template<typename T>
  concept is_view_pos_provider = requires(T const t) {
    { t.get_view_pos() } -> std::convertible_to<vec3>;
  };

  template<typename T>
  concept is_model_stack_provider = requires(T t) {
    { t.model_ref() } -> std::convertible_to<mat4&>;
    { t.push_model() };
    { t.pop_model() };
  };

  template<typename T>
  concept is_palette_provider = requires(T const t) {
    { t.palette } -> std::convertible_to<vector<vec4>>;
  };
}