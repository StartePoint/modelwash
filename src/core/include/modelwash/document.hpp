#pragma once

#include "modelwash/types.hpp"

#include <unordered_map>
#include <vector>

namespace modelwash {

class Document {
 public:
  NodeId addNode(SceneNode node);
  bool removeNode(NodeId id);
  SceneNode* findNode(NodeId id);
  const SceneNode* findNode(NodeId id) const;
  const std::vector<SceneNode>& nodes() const { return nodes_; }
  std::vector<SceneNode>& nodes() { return nodes_; }

  MaterialId addMaterial(Material mat);
  const Material* findMaterial(MaterialId id) const;
  Material* findMaterial(MaterialId id);
  const std::vector<Material>& materials() const { return materials_; }

  TextureId addTexture(Texture tex);
  const Texture* findTexture(TextureId id) const;
  const std::vector<Texture>& textures() const { return textures_; }
  std::vector<Texture>& textures() { return textures_; }

  void clear();
  void recomputeStats();
  const DocumentMeta& meta() const { return meta_; }
  DocumentMeta& meta() { return meta_; }

  void setSelection(std::vector<NodeId> ids) { selection_ = std::move(ids); }
  const std::vector<NodeId>& selection() const { return selection_; }

  Document clone() const;

 private:
  NodeId nextNodeId_ = 1;
  MaterialId nextMaterialId_ = 1;
  TextureId nextTextureId_ = 1;
  std::vector<SceneNode> nodes_;
  std::vector<Material> materials_;
  std::vector<Texture> textures_;
  std::vector<NodeId> selection_;
  DocumentMeta meta_;
};

}  // namespace modelwash
