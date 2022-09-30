#pragma once
#include <filesystem>
#include <stdexcept>
#include <vector>

#include "glm/glm.hpp"
#include "primitive.h"
#include "shape.h"
#include "spdlog/spdlog.h"
#include "texture.h"
#include "tiny_obj_loader.h"

struct Scene {
  Scene() {}

  // load obj model with tinyobjloader
  void loadObj(const std::filesystem::path& filepath)
  {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config)) {
      if (!reader.Error().empty()) {
        spdlog::error("[tinyobjloader] {}", reader.Error());
      }
      throw std::runtime_error("failed to load " + filepath.generic_string());
    }

    if (!reader.Warning().empty()) {
      spdlog::warn("[tinyobjloader] {}", reader.Warning());
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();

    // load textures
    const auto loadTexture = [&](const std::string& texture_filepath) {
      if (m_unique_textures.count(texture_filepath) == 0) {
        const int texture_id = m_textures.size();
        m_textures.emplace_back(texture_filepath);
        m_unique_textures[texture_filepath] = texture_id;
      }
    };
    for (const auto& m : materials) {
      // diffuse texture
      if (!m.diffuse_texname.empty()) {
        loadTexture(filepath.parent_path() / m.diffuse_texname);
      }
      // specular texture
      if (!m.specular_texname.empty()) {
        loadTexture(filepath.parent_path() / m.specular_texname);
      }
      // emission texture
      if (!m.emissive_texname.empty()) {
        loadTexture(filepath.parent_path() / m.emissive_texname);
      }
    }

    // load materials
    for (const auto& m : materials) {
      const Material material = loadMaterial(m, filepath.parent_path());
      m_materials.push_back(material);
    }

    // shapes loop
    for (size_t s = 0; s < shapes.size(); ++s) {
      size_t index_offset = 0;

      // triangle face loop
      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
        size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

        if (fv != 3) { throw std::runtime_error("not triangulated face"); }

        std::vector<glm::vec3> tri_vertices;
        std::vector<glm::vec3> tri_normals;
        std::vector<glm::vec2> tri_texcoords;

        // triangle vertex loop
        for (size_t v = 0; v < fv; ++v) {
          // vertex position
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
          tinyobj::real_t vx =
              attrib.vertices[3 * size_t(idx.vertex_index) + 0];
          tinyobj::real_t vy =
              attrib.vertices[3 * size_t(idx.vertex_index) + 1];
          tinyobj::real_t vz =
              attrib.vertices[3 * size_t(idx.vertex_index) + 2];
          tri_vertices.push_back(glm::vec3(vx, vy, vz));

          // vertex normal
          if (idx.normal_index >= 0) {
            tinyobj::real_t nx =
                attrib.normals[3 * size_t(idx.normal_index) + 0];
            tinyobj::real_t ny =
                attrib.normals[3 * size_t(idx.normal_index) + 1];
            tinyobj::real_t nz =
                attrib.normals[3 * size_t(idx.normal_index) + 2];
            tri_normals.push_back(glm::vec3(nx, ny, nz));
          }

          // vertex texcoord
          if (idx.texcoord_index >= 0) {
            tinyobj::real_t tx =
                attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
            tinyobj::real_t ty =
                attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
            tri_texcoords.push_back(glm::vec2(tx, ty));
          }
        }
        index_offset += fv;

        // when there is no vertex normal, use face normal instead
        if (tri_normals.size() == 0) {
          const glm::vec3 face_normal =
              glm::normalize(glm::cross(tri_vertices[1] - tri_vertices[0],
                                        tri_vertices[2] - tri_vertices[0]));
          tri_normals.push_back(face_normal);
          tri_normals.push_back(face_normal);
          tri_normals.push_back(face_normal);
        }

        // when there is no vertex texcoord, use barycentric coordinate instead
        if (tri_texcoords.size() == 0) {
          tri_texcoords.push_back(glm::vec2(0, 0));
          tri_texcoords.push_back(glm::vec2(1, 0));
          tri_texcoords.push_back(glm::vec2(0, 1));
        }

        // load triangles
        m_triangles.emplace_back(
            tri_vertices[0], tri_vertices[1], tri_vertices[2], tri_normals[0],
            tri_normals[1], tri_normals[2], tri_texcoords[0], tri_texcoords[1],
            tri_texcoords[2]);

        // load material id
        const int material_id = shapes[s].mesh.material_ids[f];
        m_material_ids.push_back(material_id);
      }
    }

    // load primitives
    for (uint32_t f = 0; f < m_triangles.size(); ++f) {
      const int material_id = m_material_ids[f];
      m_primitives.emplace_back(&m_triangles[f], &m_materials[material_id]);
    }

    spdlog::info("[Scene] number of primitives: {}", m_primitives.size());
    spdlog::info("[Scene] number of materials: {}", m_materials.size());
    spdlog::info("[Scene] number of textures: {}", m_textures.size());
  }

  // convert tinyobj::material_t to Material
  Material loadMaterial(const tinyobj::material_t& m,
                        const std::filesystem::path& parent_path) const
  {
    Material mat;

    mat.kd = glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
    mat.ks = glm::vec3(m.specular[0], m.specular[1], m.specular[2]);
    mat.ke = glm::vec3(m.emission[0], m.emission[1], m.emission[2]);

    if (!m.diffuse_texname.empty()) {
      const int texture_id =
          m_unique_textures.at(parent_path / m.diffuse_texname);
      mat.kd_tex = &m_textures[texture_id];
    }

    if (!m.specular_texname.empty()) {
      const int texture_id =
          m_unique_textures.at(parent_path / m.specular_texname);
      mat.ks_tex = &m_textures[texture_id];
    }

    if (!m.emissive_texname.empty()) {
      const int texture_id =
          m_unique_textures.at(parent_path / m.emissive_texname);
      mat.ke_tex = &m_textures[texture_id];
    }

    return mat;
  }

  // array of triangles
  std::vector<Triangle> m_triangles;

  // used for creating unique textures
  // key: texture filepath, value: texture id
  std::map<std::string, int> m_unique_textures;

  // array of textures
  std::vector<Texture> m_textures;

  // array of materials
  std::vector<Material> m_materials;

  // used for creating primitive
  std::vector<int> m_material_ids;

  // array of primitives
  std::vector<Primitive> m_primitives;
};