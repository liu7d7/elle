#include "layout.h"

namespace engine {

  layout::layout(string const& path) {
    auto toml = cpptoml::parse_file(path);
    unordered_map<string, vector<shared_ptr<joint>>> orphans;

    for (auto const& [k, v] : *toml) {
      auto j = std::make_shared<joint>(k);
      auto table = v->as_table();
      j->offset = float(table->get_as<double>("offset").value_or(0.));
      j->dark = table->get_as<bool>("dark").value_or(true);
      j->visible = table->get_as<bool>("visible").value_or(true);
      j->width = float(table->get_as<double>("width").value_or(0.03));

      if (auto parent = table->get_as<std::string>("parent")) {
        orphans[*parent].push_back(j);
      } else {
        root = j;
      }

      if (auto obj = table->get_table("obj")) {
        if (auto obj_path = obj->get_as<std::string>("path")) {
          vec3 trans{0.};
          vec3 scale{1.};
          quat rot{glm::identity<quat>()};
          if (auto obj_trans = obj->get_array_of<double>("trans")) {
            trans[0] = float(obj_trans->operator[](0));
            trans[1] = float(obj_trans->operator[](1));
            trans[2] = float(obj_trans->operator[](2));
          }

          if (auto obj_scale = obj->get_array_of<double>("scale")) {
            scale[0] = float(obj_scale->operator[](0));
            scale[1] = float(obj_scale->operator[](1));
            scale[2] = float(obj_scale->operator[](2));
          }

          if (auto obj_rot = obj->get_array_of<double>("rot")) {
            rot = glm::normalize(glm::angleAxis(glm::radians(float(obj_rot->operator[](3))),
                                                vec3{float(obj_rot->operator[](0)),
                                                     float(obj_rot->operator[](1)),
                                                     float(obj_rot->operator[](2))}));
          }

          j->obj.emplace(object{"res/models/" + *obj_path}, trans, scale, rot);
          COUT("loaded " << *obj_path << '\n')
        }
      }

      joints[k] = j;
    }

    for (auto const& [k, v] : orphans) {
      for (auto const& c : v) {
        joints[k]->link(c);
      }
    }
  }

  void layout::pose(struct pose const& p) {
    for (auto const& [k, v] : p.rotations) {
      joints[k]->rot = v;
    }
  }

  void layout::pose_lerp(struct pose const& begin, struct pose const& end, float delta) {
    for (auto const& [k, joint] : joints) {
      constexpr auto unit_y = vec3{0, 1, 0};
      auto q_u = at_or(begin.rotations, k, glm::identity<quat>());
      auto q_v = at_or(end.rotations, k, glm::identity<quat>());
//      auto u = q_u * unit_y;
//      auto v = q_v * unit_y;
//      auto axis = normalize(cross(u, v));
//      auto angle = acos(dot(u, v)) * glm::clamp(delta, 0.f, 1.f);
//      auto final = glm::angleAxis(angle, axis) * q_u;
//      joint->rot = final;
      joint->rot = glm::slerp(q_u, q_v, delta);
    }
  }

  struct pose layout::extract_pose() {
    struct pose p;
    for (const auto& [k, v] : joints) {
      p.rotations[k] = extract_rotation(v->fin_rot);
    }

    return p;
  }
}
