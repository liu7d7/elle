#include "object.h"

namespace engine {
  static unordered_map<string, texture_ext> textures_loaded;

  struct object_state object_state;

  vertex::vertex(vec3 pos, vec3 norm, vec2 tex_coord) :
    pos(pos), norm(norm), tex_coord(tex_coord) { }

  object::object(string const& path) : path(path) {
    if (!std::filesystem::exists(std::filesystem::path(path.c_str()))) {
      COUT("ERROR::OBJECT::FILE_NOT_FOUND: " << path << '\n')
      return;
    }

    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      COUT("ERROR::ASSIMP:: " << importer.GetErrorString() << '\n')
      return;
    }

    string dir = path.substr(0, path.find_last_of('/'));
    process_node(scene->mRootNode, scene, dir);
  }

  void object::process_node(aiNode* node, aiScene const* scene, string const& dir) {
    for (uint i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      groups.push_back(process_mesh(mesh, scene, dir));
    }

    for (uint i = 0; i < node->mNumChildren; i++) {
      process_node(node->mChildren[i], scene, dir);
    }
  }

  group object::process_mesh(aiMesh* mesh, aiScene const* scene, string const& dir) {
    vector<vertex> verts;
    vector<uint> indices;

    for (uint i = 0; i < mesh->mNumVertices; i++) {
      verts.emplace_back(
        vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
        vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
        mesh->mTextureCoords[0] ? vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : vec2(0.));
    }

    for (uint i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (uint j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }

    linked_vertex_array vao({3, 3, 2});

    vao.set_vertex_data(verts);
    vao.set_index_data(indices);

    vector<texture_ext> textures;
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    load_textures(textures, mat, aiTextureType_DIFFUSE, dir);
    load_textures(textures, mat, aiTextureType_SPECULAR, dir);
    load_textures(textures, mat, aiTextureType_NORMALS, dir);
    load_textures(textures, mat, aiTextureType_HEIGHT, dir);

    aiColor3D ambient;
    mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    aiColor3D diffuse;
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    aiColor3D specular;
    mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    float shininess;
    mat->Get(AI_MATKEY_SHININESS, shininess);
    int illum;
    mat->Get(AI_MATKEY_SHADING_MODEL, illum);

    lighting light{
      vec3(ambient.r, ambient.g, ambient.b),
      vec3(diffuse.r, diffuse.g, diffuse.b),
      vec3(specular.r, specular.g, specular.b),
      shininess,
      illum
    };

    return {mesh->mName.C_Str(), vao, textures, light};
  }

  void object::load_textures(vector<texture_ext>& textures, aiMaterial* mat, aiTextureType type, string const& dir) {
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
      aiString str;
      mat->GetTexture(type, i, &str);

      if (!textures_loaded.contains(str.C_Str())) {
        textures.emplace_back(texture(dir + '/' + str.C_Str(), GL_LINEAR, GL_LINEAR), type);
        textures_loaded.emplace(str.C_Str(), textures.back());
      } else {
        textures.push_back(textures_loaded.at(str.C_Str()));
      }
    }
  }
}