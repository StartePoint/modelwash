#include "modelwash/document.hpp"

#include <algorithm>

namespace modelwash {

NodeId Document::addNode(SceneNode node) {
  if (node.id == 0) {
    node.id = nextNodeId_++;
  } else {
    nextNodeId_ = std::max(nextNodeId_, node.id + 1);
  }
  nodes_.push_back(std::move(node));
  return nodes_.back().id;
}

bool Document::removeNode(NodeId id) {
  const auto it = std::remove_if(nodes_.begin(), nodes_.end(),
                                 [&](const SceneNode& n) { return n.id == id; });
  if (it == nodes_.end()) {
    return false;
  }
  nodes_.erase(it, nodes_.end());
  selection_.erase(std::remove(selection_.begin(), selection_.end(), id),
                   selection_.end());
  return true;
}

SceneNode* Document::findNode(NodeId id) {
  for (auto& n : nodes_) {
    if (n.id == id) {
      return &n;
    }
  }
  return nullptr;
}

const SceneNode* Document::findNode(NodeId id) const {
  for (const auto& n : nodes_) {
    if (n.id == id) {
      return &n;
    }
  }
  return nullptr;
}

MaterialId Document::addMaterial(Material mat) {
  if (mat.id == 0) {
    mat.id = nextMaterialId_++;
  } else {
    nextMaterialId_ = std::max(nextMaterialId_, mat.id + 1);
  }
  materials_.push_back(std::move(mat));
  return materials_.back().id;
}

const Material* Document::findMaterial(MaterialId id) const {
  for (const auto& m : materials_) {
    if (m.id == id) {
      return &m;
    }
  }
  return nullptr;
}

Material* Document::findMaterial(MaterialId id) {
  for (auto& m : materials_) {
    if (m.id == id) {
      return &m;
    }
  }
  return nullptr;
}

TextureId Document::addTexture(Texture tex) {
  if (tex.id == 0) {
    tex.id = nextTextureId_++;
  } else {
    nextTextureId_ = std::max(nextTextureId_, tex.id + 1);
  }
  textures_.push_back(std::move(tex));
  return textures_.back().id;
}

const Texture* Document::findTexture(TextureId id) const {
  for (const auto& t : textures_) {
    if (t.id == id) {
      return &t;
    }
  }
  return nullptr;
}

void Document::clear() {
  nodes_.clear();
  materials_.clear();
  textures_.clear();
  selection_.clear();
  meta_ = {};
  nextNodeId_ = 1;
  nextMaterialId_ = 1;
  nextTextureId_ = 1;
}

void Document::recomputeStats() {
  meta_.triangleCount = 0;
  meta_.approxTextureBytes = 0;
  meta_.hasSkinning = false;
  meta_.hasMorph = false;
  for (const auto& n : nodes_) {
    if (!n.mesh) {
      continue;
    }
    meta_.triangleCount += n.mesh->triangleCount();
    if (hasFlag(n.mesh->flags, MeshFlags::Skinned)) {
      meta_.hasSkinning = true;
    }
    if (hasFlag(n.mesh->flags, MeshFlags::HasMorph)) {
      meta_.hasMorph = true;
    }
  }
  for (const auto& t : textures_) {
    meta_.approxTextureBytes += t.pixels.size();
  }
  meta_.warnings.clear();
  if (meta_.triangleCount > 500000) {
    meta_.warnings.push_back(WarningCode::StrongFaceBudget);
  } else if (meta_.triangleCount > 150000) {
    meta_.warnings.push_back(WarningCode::SoftFaceBudget);
  }
}

Document Document::clone() const {
  Document d;
  d.nodes_ = nodes_;
  d.materials_ = materials_;
  d.textures_ = textures_;
  d.selection_ = selection_;
  d.meta_ = meta_;
  d.nextNodeId_ = nextNodeId_;
  d.nextMaterialId_ = nextMaterialId_;
  d.nextTextureId_ = nextTextureId_;
  return d;
}

}  // namespace modelwash
