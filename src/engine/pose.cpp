#include "pose.h"

namespace engine {
  shared_ptr<cpptoml::table> pose::as_toml() const {
    auto root = cpptoml::make_table();
    for (auto const& [k, v] : rotations) {
      auto table = cpptoml::make_table();
      vec3 ax = glm::axis(v);
      auto arr = cpptoml::make_array();
      arr->push_back(ax.x);
      arr->push_back(ax.y);
      arr->push_back(ax.z);
      arr->push_back(glm::degrees(glm::angle(v)));
      table->insert("rotation", arr);
      root->insert(k, table);
    }
    return root;
  }

  pose::pose(string const& s) {
    auto toml = cpptoml::parse_file(s);
    for (auto const& [k, v] : *toml) {
      auto table = v->as_table();
      if (auto arr = table->get_array_of<double>("rotation")) {
        rotations[k] = glm::angleAxis(glm::radians(float(arr->operator[](3))), normalize(vec3{float(arr->operator[](0)),
                                                                                    float(arr->operator[](1)),
                                                                                    float(arr->operator[](2))}));
      } else if (auto change = table->get_array_of<double>("change")) {
        vec3 chg = normalize(vec3{float(change->operator[](0)), float(change->operator[](1)), float(change->operator[](2))});
        auto ax = int(table->get_as<long long>("ax").value_or(1));
        mat4 c = change_axis(chg, ax);
        rotations[k] = extract_rotation(c);
      }
    }
  }
}