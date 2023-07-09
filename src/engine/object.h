#pragma once

#include "usings.h"
#include "vertex_array.h"
#include "linked_vertex_array.h"
#include "texture.h"
#include "utils.h"
#include "shader.h"
#include "lazy.h"
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace engine {
  struct vertex {
    vec3 pos;
    vec3 norm;
    vec2 tex_coord;

    vertex(vec3 pos, vec3 norm, vec2 tex_coord);
  };

  struct lighting {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    int illum;
  };

  using texture_ext = tuple<texture, aiTextureType>;
  using group = tuple<string, linked_vertex_array, vector<texture_ext>, lighting>;

  struct object_state {
    lazy<shader, string, string> sh{"res/shaders/mod.vert", "res/shaders/mod.frag"};
  };

  extern struct object_state object_state;

  struct object {
    vector<group> groups;
    string path;

    explicit object(string const& path);

  private:
    void process_node(aiNode* node, aiScene const* scene, string const& dir);

    group process_mesh(aiMesh* mesh, aiScene const* scene, string const& dir);

    static void load_textures(vector<texture_ext>& textures, aiMaterial* mat, aiTextureType type, string const& dir);
  };

  template<typename T>
  requires is_mvp_provider<T> && is_view_pos_provider<T>
  void draw_obj(T const& s, object const& obj) {
    auto& sh = *object_state.sh;
    gl_use_program(sh);
    sh.set("proj", s.get_proj());
    sh.set("look", s.get_view());
    sh.set("model", s.get_model());
    sh.set("view_pos", s.get_view_pos());

    auto texture = [i = 0](array<bool, 3>& has_x, uint handle, aiTextureType type) mutable {
      if (!has_x[type - 1]) {
        has_x[type - 1] = true;
        gl_active_texture(GL_TEXTURE0 + i);
        gl_bind_texture(GL_TEXTURE_2D, handle);
        ++i;
      }
    };

    for (auto const& [n, va, ts, l] : obj.groups) {
      array<bool, 3> has_x{};
      for (auto const& [t, type] : ts) {
        texture(has_x, t.handle, type);
      }

      sh.set("ka", l.ambient);
      sh.set("kd", l.diffuse);
      sh.set("ks", l.specular);
      sh.set("shininess", l.shininess);
      sh.set("has_map_ka", int(has_x[aiTextureType_AMBIENT - 1]));
      sh.set("has_map_kd", int(has_x[aiTextureType_DIFFUSE - 1]));
      sh.set("has_map_ks", int(has_x[aiTextureType_SPECULAR - 1]));

      gl_bind_vertex_array(va);
      gl_draw_elements(GL_TRIANGLES, va.size, GL_UNSIGNED_INT, nullptr);
    }
  }
}