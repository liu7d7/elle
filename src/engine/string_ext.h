#pragma once

#include <string>

namespace engine {
  struct string_ext : public std::string {
    string_ext() : std::string() {}

    /* implicit */ string_ext(const char* c) : string_ext(std::string(c)) {}

    /* implicit */ string_ext(std::string const& s) : std::string(s), m_hash(std::hash<std::string>()(s)) {}

  private:
    size_t m_hash;

    friend struct std::hash<string_ext>;
  };
}

template<>
struct std::hash<engine::string_ext> {
  size_t operator ()(engine::string_ext const& str) const {
    return str.m_hash;
  }
};