#include "modelwash/io.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <QImage>
#include <QFileInfo>
#include <QDir>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

namespace modelwash {
namespace {

std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return s;
}

Texture textureFromQImage(QImage img, const std::string& name) {
  Texture tex;
  tex.name = name;
  img = img.convertToFormat(QImage::Format_RGBA8888);
  tex.width = img.width();
  tex.height = img.height();
  tex.channels = 4;
  tex.pixels.resize(static_cast<std::size_t>(tex.width) * tex.height * 4);
  // QImage may have padding; copy scanline by scanline.
  for (int y = 0; y < tex.height; ++y) {
    const uchar* line = img.constScanLine(y);
    std::memcpy(tex.pixels.data() + static_cast<std::size_t>(y) * tex.width * 4, line,
                static_cast<std::size_t>(tex.width) * 4);
  }
  return tex;
}

bool loadAssimpTexture(const aiScene* scene, const std::filesystem::path& modelPath,
                       const aiString& texPath, Texture& out) {
  const std::string pathStr = texPath.C_Str();
  if (pathStr.empty()) {
    return false;
  }

  const aiTexture* emb = scene->GetEmbeddedTexture(pathStr.c_str());
  if (!emb && pathStr[0] == '*') {
    // Fallback: "*N" index into scene->mTextures
    char* end = nullptr;
    const long idx = std::strtol(pathStr.c_str() + 1, &end, 10);
    if (idx >= 0 && idx < static_cast<long>(scene->mNumTextures)) {
      emb = scene->mTextures[idx];
    }
  }

  if (emb) {
    if (emb->mHeight == 0) {
      QImage img;
      const QByteArray bytes(reinterpret_cast<const char*>(emb->pcData),
                             static_cast<int>(emb->mWidth));
      // Prefer format hint (png/jpg/webp); fall back to autodetection.
      if (!img.loadFromData(bytes, emb->achFormatHint) && !img.loadFromData(bytes)) {
        return false;
      }
      out = textureFromQImage(img, pathStr);
      return true;
    }
    QImage img(static_cast<int>(emb->mWidth), static_cast<int>(emb->mHeight),
               QImage::Format_RGBA8888);
    for (unsigned y = 0; y < emb->mHeight; ++y) {
      auto* dst = reinterpret_cast<uchar*>(img.scanLine(static_cast<int>(y)));
      for (unsigned x = 0; x < emb->mWidth; ++x) {
        const aiTexel& t = emb->pcData[y * emb->mWidth + x];
        dst[x * 4 + 0] = t.r;
        dst[x * 4 + 1] = t.g;
        dst[x * 4 + 2] = t.b;
        dst[x * 4 + 3] = t.a;
      }
    }
    out = textureFromQImage(img, pathStr);
    return true;
  }

  std::filesystem::path resolved(pathStr);
  if (resolved.is_relative()) {
    resolved = modelPath.parent_path() / resolved;
  }
  QImage img(QString::fromStdWString(resolved.wstring()));
  if (img.isNull()) {
    img = QImage(QString::fromStdString(resolved.string()));
  }
  if (img.isNull()) {
    // Try basename next to the model (common for OBJ/MTL).
    const auto base = modelPath.parent_path() / resolved.filename();
    img = QImage(QString::fromStdWString(base.wstring()));
  }
  if (img.isNull()) {
    return false;
  }
  out = textureFromQImage(img, pathStr);
  return true;
}

class AssimpAdapter final : public IFormatAdapter {
 public:
  std::string formatId() const override { return "assimp"; }

  bool canOpen(const std::filesystem::path& path) const override {
    const auto ext = toLower(path.extension().string());
    return ext == ".glb" || ext == ".gltf" || ext == ".obj" || ext == ".fbx" ||
           ext == ".dae" || ext == ".ply" || ext == ".stl";
  }

  ImportResult importFile(const ImportRequest& req) const override {
    ImportResult out;
    const auto ext = toLower(req.path.extension().string());

    Assimp::Importer importer;
    const unsigned flags =
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals | aiProcess_ImproveCacheLocality |
        aiProcess_SortByPType | aiProcess_PreTransformVertices;

    const aiScene* scene = importer.ReadFile(req.path.string(), flags);
    if (!scene || !scene->mRootNode) {
      out.messageZh = std::string("Import failed: ") + importer.GetErrorString() +
                      ". Try converting to GLB.";
      return out;
    }

    Document doc;
    doc.meta().sourcePath = req.path.string();
    doc.meta().hasAnimation = scene->mNumAnimations > 0;

    std::unordered_map<std::string, TextureId> textureCache;

    auto ensureTexture = [&](const aiString& path) -> std::optional<TextureId> {
      const std::string key = path.C_Str();
      if (key.empty()) {
        return std::nullopt;
      }
      if (auto it = textureCache.find(key); it != textureCache.end()) {
        return it->second;
      }
      Texture tex;
      if (!loadAssimpTexture(scene, req.path, path, tex)) {
        return std::nullopt;
      }
      const TextureId id = doc.addTexture(std::move(tex));
      textureCache.emplace(key, id);
      return id;
    };

    // Preload every embedded texture so "*N" material refs always resolve.
    for (unsigned ti = 0; ti < scene->mNumTextures; ++ti) {
      aiString key;
      key.Set(("*" + std::to_string(ti)).c_str());
      (void)ensureTexture(key);
    }

    for (unsigned mi = 0; mi < scene->mNumMaterials; ++mi) {
      const aiMaterial* aim = scene->mMaterials[mi];
      Material mat;
      mat.name = aim->GetName().C_Str();

      aiColor4D color(1, 1, 1, 1);
      if (AI_SUCCESS == aim->Get(AI_MATKEY_BASE_COLOR, color) ||
          AI_SUCCESS == aim->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
        mat.baseColorFactor = {color.r, color.g, color.b, color.a};
      }

      float metallic = 0.f;
      float roughness = 0.5f;
      aim->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
      aim->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
      mat.metallic = metallic;
      mat.roughness = roughness;

      aiString texPath;
      bool gotTex = false;
      const aiTextureType texTypes[] = {
          aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE, aiTextureType_UNKNOWN,
          aiTextureType_EMISSIVE,
      };
      for (aiTextureType tt : texTypes) {
        if (aim->GetTextureCount(tt) == 0) {
          continue;
        }
        if (AI_SUCCESS == aim->GetTexture(tt, 0, &texPath)) {
          gotTex = true;
          break;
        }
      }
      if (gotTex) {
        if (auto tid = ensureTexture(texPath)) {
          mat.baseColorTexture = *tid;
        }
      }

      doc.addMaterial(std::move(mat));
    }

    for (unsigned meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
      const aiMesh* aim = scene->mMeshes[meshIndex];
      SceneNode node;
      node.name = aim->mName.C_Str();
      if (node.name.empty()) {
        node.name = "Mesh_" + std::to_string(meshIndex);
      }
      MeshData mesh;
      mesh.positions.resize(aim->mNumVertices);
      for (unsigned i = 0; i < aim->mNumVertices; ++i) {
        mesh.positions[i] = {aim->mVertices[i].x, aim->mVertices[i].y,
                             aim->mVertices[i].z};
      }
      if (aim->HasNormals()) {
        mesh.normals.resize(aim->mNumVertices);
        for (unsigned i = 0; i < aim->mNumVertices; ++i) {
          mesh.normals[i] = {aim->mNormals[i].x, aim->mNormals[i].y,
                             aim->mNormals[i].z};
        }
      }
      if (aim->HasTextureCoords(0)) {
        mesh.uvs.resize(aim->mNumVertices);
        for (unsigned i = 0; i < aim->mNumVertices; ++i) {
          mesh.uvs[i] = {aim->mTextureCoords[0][i].x, aim->mTextureCoords[0][i].y};
        }
      }
      mesh.indices.reserve(aim->mNumFaces * 3);
      for (unsigned f = 0; f < aim->mNumFaces; ++f) {
        const aiFace& face = aim->mFaces[f];
        if (face.mNumIndices != 3) {
          continue;
        }
        mesh.indices.push_back(face.mIndices[0]);
        mesh.indices.push_back(face.mIndices[1]);
        mesh.indices.push_back(face.mIndices[2]);
      }
      if (aim->mNumBones > 0) {
        mesh.flags = mesh.flags | MeshFlags::Skinned;
        doc.meta().hasSkinning = true;
      }
      if (aim->mNumAnimMeshes > 0) {
        mesh.flags = mesh.flags | MeshFlags::HasMorph;
        doc.meta().hasMorph = true;
      }
      node.mesh = std::move(mesh);
      if (aim->mMaterialIndex < doc.materials().size()) {
        node.materialId = static_cast<MaterialId>(aim->mMaterialIndex + 1);
      }
      doc.addNode(std::move(node));
    }

    if (doc.nodes().empty()) {
      out.messageZh = "No triangle meshes found.";
      return out;
    }

    if (ext != ".glb" && ext != ".gltf" && ext != ".obj") {
      doc.meta().warnings.push_back(WarningCode::ImportPartial);
    }

    doc.recomputeStats();
    out.ok = true;
    out.document = std::move(doc);
    out.messageZh = "Import OK.";
    return out;
  }
};

}  // namespace

std::unique_ptr<IFormatAdapter> makeAssimpAdapter() {
  return std::make_unique<AssimpAdapter>();
}

void FormatRegistry::add(std::unique_ptr<IFormatAdapter> adapter) {
  adapters_.push_back(std::move(adapter));
}

ImportResult FormatRegistry::open(const std::filesystem::path& path) const {
  ImportResult fail;
  const auto ext = toLower(path.extension().string());
  for (const auto& a : adapters_) {
    if (a->canOpen(path)) {
      return a->importFile(ImportRequest{path});
    }
  }
  fail.messageZh = "Unsupported format. Convert to GLB first. ";
  if (!ext.empty()) {
    fail.messageZh += "（" + ext + "）";
  }
  return fail;
}

}  // namespace modelwash
