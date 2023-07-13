#include "editor.h"
#include <fstream>
#include "engine/pose.h"
#include "glm/detail/type_quat.hpp"
#include "engine/utils.h"
#include "engine/usings.h"

namespace elle {
  void write_pose(string const& name, pose const& pose) {
    std::ofstream ofs(name);
    ofs << *pose.as_toml();
    COUT(*pose.as_toml() << std::endl)
    ofs.flush();
    ofs.close();
  }

  void absolute(state& s, string const& editing) {
    std::istringstream iss(editing);
    string _;
    iss >> _;
    vec3 v;
    iss >> v.x >> v.y >> v.z;
    constexpr auto u = vec3{0, 1, 0};
    quat q = u_to_v(u, v);
    s.poses.at(s.layout_editor.current_pose).rotations[s.layout_editor.current_joint] = q;
    s.layout.pose(s.poses.at(s.layout_editor.current_pose));
    COUT(glm::axis(q) << " " << glm::degrees(glm::angle(q)) << " " << u << " " << v << std::endl)
  }

  void impactful(state& s, string const& editing) {
    std::stringstream iss{editing};
    std::string i, joint_id;
    iss >> i >> joint_id;
    float x, y, z, ang;
    iss >> x >> y >> z >> ang;
    COUT(joint_id << ' ' << x << ' ' << y << ' ' << z << ' ' << ang << std::endl)
    quat q = glm::angleAxis(glm::radians(ang), vec3{x, y, z});
    stack<shared_ptr<joint>> joints;
    joints.push(s.layout.at(joint_id));
    pose& p = s.poses.at(s.layout_editor.current_pose);
    while (!joints.empty()) {
      auto f = joints.top();
      joints.pop();
      quat& q1 = p.rotations.at(f->name);
      COUT(f->name << std::endl)
      q1 = q * q1;
      for (const auto& it : f->children) {
        joints.push(it);
      }
    }
    s.layout.pose(p);
  }

  bool editor(application& m) {
    if (m.state.has_value()) {
      state& s = m.state.value();
      string editing;
      std::getline(std::cin, editing);
      editing = trim(editing);

      if (editing.starts_with("absolute") && s.layout.joints.contains(s.layout_editor.current_joint) &&
          s.poses.contains(s.layout_editor.current_pose)) {
        absolute(s, editing);
      } else if (editing.starts_with("save") && s.poses.contains(s.layout_editor.current_pose)) {
        string name = trim(editing.substr(4));
        write_pose(name, s.poses.at(s.layout_editor.current_pose));
      } else if (editing.starts_with("impactful")) {
        impactful(s, editing);
      } else if (editing.starts_with("quit")) {
        glfw_set_window_should_close(m, true);
        return true;
      } else if (editing.starts_with("edit")) {
        editing = trim(editing.substr(4));
        if (s.poses.contains(editing)) {
          s.layout_editor.current_pose = editing;
          s.layout.pose(s.poses.at(editing));
          COUT("editing " << editing << std::endl)
        } else {
          COUT("not a joint" << std::endl)
          s.layout_editor.current_pose.clear();
        }
      } else if (editing.starts_with("joint")) {
        editing = trim(editing.substr(5));
        if (s.layout.joints.contains(editing)) {
          s.layout_editor.current_joint = editing;
          COUT("editing joint " << editing << std::endl)
        } else {
          COUT("not a joint" << std::endl)
          s.layout_editor.current_joint.clear();
        }
      }
    }
    return false;
  }
}