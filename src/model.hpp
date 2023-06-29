#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "mesh.hpp"
#include "shader.hpp"

unsigned int load_texture_from_file(const char* path,
                                    const std::string& directory,
                                    bool gamma = false);

class Model {
 public:
  std::vector<Texture> loaded_textures;
  std::vector<Mesh> meshes;
  std::string directory;
  bool gamma_correction;

  Model(std::string const& path, bool gamma = false) : gamma_correction(gamma) {
    load_model(path);
  }

  void draw(Shader& shader) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
      meshes[i].draw(shader);
    }
  }

 private:
  void load_model(std::string const& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                  aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      std::cerr << "ERROR::ASSIMP\n" << importer.GetErrorString() << "\n";
      return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene);
  }

  void process_node(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(process_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      process_node(node->mChildren[i], scene);
    }
  }

  Mesh process_mesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      glm::vec3 vec3_temp;

      vec3_temp.x = mesh->mVertices[i].x;
      vec3_temp.y = mesh->mVertices[i].y;
      vec3_temp.z = mesh->mVertices[i].z;

      vertex.position = vec3_temp;

      if (mesh->HasNormals()) {
        vec3_temp.x = mesh->mNormals[i].x;
        vec3_temp.y = mesh->mNormals[i].y;
        vec3_temp.z = mesh->mNormals[i].z;

        vertex.normal = vec3_temp;
      }

      if (mesh->mTextureCoords[0]) {
        glm::vec2 vec2_temp;

        vec2_temp.x = mesh->mTextureCoords[0][i].x;
        vec2_temp.y = mesh->mTextureCoords[0][i].y;

        vertex.tex_coords = vec2_temp;

        vec3_temp.x = mesh->mTangents[i].x;
        vec3_temp.y = mesh->mTangents[i].y;
        vec3_temp.z = mesh->mTangents[i].z;

        vertex.tangent = vec3_temp;

        vec3_temp.x = mesh->mBitangents[i].x;
        vec3_temp.y = mesh->mBitangents[i].y;
        vec3_temp.z = mesh->mBitangents[i].z;

        vertex.bitangent = vec3_temp;
      } else {
        vertex.tex_coords = glm::vec2(0.0f, 0.0f);
      }

      vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];

      for (unsigned int j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuse_maps = load_material_textures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    std::vector<Texture> specular_maps = load_material_textures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

    std::vector<Texture> normal_maps = load_material_textures(
        material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

    std::vector<Texture> height_maps = load_material_textures(
        material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), height_maps.begin(), height_maps.end());

    return Mesh(vertices, indices, textures);
  }

  std::vector<Texture> load_material_textures(aiMaterial* material,
                                              aiTextureType type,
                                              std::string type_name) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
      aiString str;
      material->GetTexture(type, i, &str);

      bool skip = false;

      for (unsigned int j = 0; j < loaded_textures.size(); j++) {
        if (std::strcmp(loaded_textures[j].path.data(), str.C_Str()) == 0) {
          textures.push_back(loaded_textures[j]);
          skip = true;
          break;
        }
      }

      if (!skip) {
        Texture texture;
        texture.id = load_texture_from_file(str.C_Str(), this->directory);
        texture.type = type_name;
        texture.path = str.C_Str();
        textures.push_back(texture);
        loaded_textures.push_back(texture);
      }
    }

    return textures;
  }
};

unsigned int load_texture_from_file(const char* path,
                                    const std::string& directory, bool gamma) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  unsigned int texture_ID;
  glGenTextures(1, &texture_ID);

  int width, height, n_components;
  unsigned char* data =
      stbi_load(filename.c_str(), &width, &height, &n_components, 0);

  if (data) {
    GLenum format;

    if (n_components == 1) {
      format = GL_RED;
    } else if (n_components == 3) {
      format = GL_RGB;
    } else if (n_components == 4) {
      format = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cerr << "Texture failed to load at path: " << path << "\n";
    stbi_image_free(data);
  }

  return texture_ID;
}
