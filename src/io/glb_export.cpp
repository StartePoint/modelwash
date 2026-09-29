#include "modelwash/io.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

namespace modelwash {
namespace {

void appendBytes(std::vector<std::uint8_t>& buf, const void* data, std::size_t n) {
  const auto* p = static_cast<const std::uint8_t*>(data);
  buf.insert(buf.end(), p, p + n);
}

void pad4(std::vector<std::uint8_t>& buf) {
  while (buf.size() % 4 != 0) {
    buf.push_back(0);
  }
}

}  // namespace

ExportResult exportGlb(const ExportRequest& req) {
  ExportResult out;
  if (!req.document) {
    out.messageZh = "Nothing to export.";
    return out;
  }
  const Document& doc = *req.document;

  // Build a single buffer with all mesh attributes interleaved loosely:
  // for each mesh: positions, normals?, uvs?, indices
  std::vector<std::uint8_t> bin;
  std::ostringstream json;
  json << std::fixed;
  json.precision(6);

  json << "{\"asset\":{\"version\":\"2.0\",\"generator\":\"ModelWash/0.1\"},";
  json << "\"scene\":0,\"scenes\":[{\"nodes\":[";

  std::vector<int> rootNodes;
  int nodeCount = 0;
  for (std::size_t i = 0; i < doc.nodes().size(); ++i) {
    if (i) json << ",";
    json << nodeCount;
    rootNodes.push_back(nodeCount);
    ++nodeCount;
  }
  json << "]}],\"nodes\":[";

  struct Acc {
    int bufferView = 0;
    int byteOffset = 0;
    int count = 0;
    int componentType = 0;
    std::string type;
    Vec3 minP{};
    Vec3 maxP{};
    bool hasMinMax = false;
  };
  std::vector<std::string> meshJsonParts;
  std::vector<std::string> nodeJsonParts;
  std::vector<Acc> accessors;
  std::vector<std::string> bufferViews;

  auto addBufferView = [&](int offset, int length, int target) {
    std::ostringstream bv;
    bv << "{\"buffer\":0,\"byteOffset\":" << offset << ",\"byteLength\":" << length;
    if (target) {
      bv << ",\"target\":" << target;
    }
    bv << "}";
    bufferViews.push_back(bv.str());
    return static_cast<int>(bufferViews.size() - 1);
  };

  auto addAccessor = [&](Acc a) {
    accessors.push_back(a);
    return static_cast<int>(accessors.size() - 1);
  };

  for (std::size_t ni = 0; ni < doc.nodes().size(); ++ni) {
    const auto& node = doc.nodes()[ni];
    std::ostringstream nj;
    nj << "{\"name\":\"";
    for (char c : node.name) {
      if (c == '"' || c == '\\') nj << '\\';
      nj << c;
    }
    nj << "\"";
    if (node.mesh && !node.mesh->positions.empty()) {
      const MeshData& mesh = *node.mesh;
      const int posOffset = static_cast<int>(bin.size());
      appendBytes(bin, mesh.positions.data(), mesh.positions.size() * sizeof(Vec3));
      const int posLen = static_cast<int>(bin.size()) - posOffset;
      const int posView = addBufferView(posOffset, posLen, 34962);

      Acc posAcc;
      posAcc.bufferView = posView;
      posAcc.count = static_cast<int>(mesh.positions.size());
      posAcc.componentType = 5126;
      posAcc.type = "VEC3";
      posAcc.hasMinMax = true;
      posAcc.minP = mesh.positions[0];
      posAcc.maxP = mesh.positions[0];
      for (const auto& p : mesh.positions) {
        posAcc.minP.x = std::min(posAcc.minP.x, p.x);
        posAcc.minP.y = std::min(posAcc.minP.y, p.y);
        posAcc.minP.z = std::min(posAcc.minP.z, p.z);
        posAcc.maxP.x = std::max(posAcc.maxP.x, p.x);
        posAcc.maxP.y = std::max(posAcc.maxP.y, p.y);
        posAcc.maxP.z = std::max(posAcc.maxP.z, p.z);
      }
      const int posAccessor = addAccessor(posAcc);

      int nrmAccessor = -1;
      if (!mesh.normals.empty() && mesh.normals.size() == mesh.positions.size()) {
        pad4(bin);
        const int off = static_cast<int>(bin.size());
        appendBytes(bin, mesh.normals.data(), mesh.normals.size() * sizeof(Vec3));
        const int view = addBufferView(off, static_cast<int>(bin.size()) - off, 34962);
        Acc a;
        a.bufferView = view;
        a.count = static_cast<int>(mesh.normals.size());
        a.componentType = 5126;
        a.type = "VEC3";
        nrmAccessor = addAccessor(a);
      }

      int uvAccessor = -1;
      if (!mesh.uvs.empty() && mesh.uvs.size() == mesh.positions.size()) {
        pad4(bin);
        const int off = static_cast<int>(bin.size());
        appendBytes(bin, mesh.uvs.data(), mesh.uvs.size() * sizeof(Vec2));
        const int view = addBufferView(off, static_cast<int>(bin.size()) - off, 34962);
        Acc a;
        a.bufferView = view;
        a.count = static_cast<int>(mesh.uvs.size());
        a.componentType = 5126;
        a.type = "VEC2";
        uvAccessor = addAccessor(a);
      }

      std::vector<std::uint32_t> indices = mesh.indices;
      if (indices.empty()) {
        indices.resize(mesh.positions.size());
        for (std::uint32_t i = 0; i < indices.size(); ++i) {
          indices[i] = i;
        }
      }
      pad4(bin);
      const int iOff = static_cast<int>(bin.size());
      appendBytes(bin, indices.data(), indices.size() * sizeof(std::uint32_t));
      const int iView = addBufferView(iOff, static_cast<int>(bin.size()) - iOff, 34963);
      Acc iAcc;
      iAcc.bufferView = iView;
      iAcc.count = static_cast<int>(indices.size());
      iAcc.componentType = 5125;
      iAcc.type = "SCALAR";
      const int iAccessor = addAccessor(iAcc);

      std::ostringstream mj;
      mj << "{\"primitives\":[{\"attributes\":{\"POSITION\":" << posAccessor;
      if (nrmAccessor >= 0) {
        mj << ",\"NORMAL\":" << nrmAccessor;
      }
      if (uvAccessor >= 0) {
        mj << ",\"TEXCOORD_0\":" << uvAccessor;
      }
      mj << "},\"indices\":" << iAccessor << "}]}";
      const int meshIndex = static_cast<int>(meshJsonParts.size());
      meshJsonParts.push_back(mj.str());
      nj << ",\"mesh\":" << meshIndex;
    }
    nj << "}";
    nodeJsonParts.push_back(nj.str());
  }

  for (std::size_t i = 0; i < nodeJsonParts.size(); ++i) {
    if (i) json << ",";
    json << nodeJsonParts[i];
  }
  json << "],\"meshes\":[";
  for (std::size_t i = 0; i < meshJsonParts.size(); ++i) {
    if (i) json << ",";
    json << meshJsonParts[i];
  }
  json << "],\"accessors\":[";
  for (std::size_t i = 0; i < accessors.size(); ++i) {
    if (i) json << ",";
    const Acc& a = accessors[i];
    json << "{\"bufferView\":" << a.bufferView << ",\"componentType\":" << a.componentType
         << ",\"count\":" << a.count << ",\"type\":\"" << a.type << "\"";
    if (a.hasMinMax) {
      json << ",\"min\":[" << a.minP.x << "," << a.minP.y << "," << a.minP.z << "],\"max\":["
           << a.maxP.x << "," << a.maxP.y << "," << a.maxP.z << "]";
    }
    json << "}";
  }
  json << "],\"bufferViews\":[";
  for (std::size_t i = 0; i < bufferViews.size(); ++i) {
    if (i) json << ",";
    json << bufferViews[i];
  }
  pad4(bin);
  json << "],\"buffers\":[{\"byteLength\":" << bin.size() << "}]}";

  std::string jsonStr = json.str();
  while (jsonStr.size() % 4 != 0) {
    jsonStr.push_back(' ');
  }

  // GLB container
  std::vector<std::uint8_t> glb;
  const char magic[4] = {'g', 'l', 'T', 'F'};
  appendBytes(glb, magic, 4);
  const std::uint32_t version = 2;
  appendBytes(glb, &version, 4);
  const std::uint32_t totalLength = static_cast<std::uint32_t>(
      12 + 8 + jsonStr.size() + 8 + bin.size());
  appendBytes(glb, &totalLength, 4);

  const std::uint32_t jsonLen = static_cast<std::uint32_t>(jsonStr.size());
  const std::uint32_t jsonType = 0x4E4F534A;  // JSON
  appendBytes(glb, &jsonLen, 4);
  appendBytes(glb, &jsonType, 4);
  appendBytes(glb, jsonStr.data(), jsonStr.size());

  const std::uint32_t binLen = static_cast<std::uint32_t>(bin.size());
  const std::uint32_t binType = 0x004E4942;  // BIN\0
  appendBytes(glb, &binLen, 4);
  appendBytes(glb, &binType, 4);
  appendBytes(glb, bin.data(), bin.size());

  // Atomic write: temp then rename
  const auto tmp = req.output;
  std::filesystem::path tmpPath = req.output;
  tmpPath += ".tmp";
  {
    std::ofstream ofs(tmpPath, std::ios::binary);
    if (!ofs) {
      out.messageZh = "Export failed: cannot write temp file.";
      return out;
    }
    ofs.write(reinterpret_cast<const char*>(glb.data()),
              static_cast<std::streamsize>(glb.size()));
    if (!ofs) {
      out.messageZh = "Export failed: write interrupted.";
      return out;
    }
  }
  std::error_code ec;
  std::filesystem::rename(tmpPath, req.output, ec);
  if (ec) {
    std::filesystem::remove(req.output, ec);
    std::filesystem::rename(tmpPath, req.output, ec);
    if (ec) {
      out.messageZh = "Export failed; file not overwritten.";
      return out;
    }
  }

  out.ok = true;
  out.bytesWritten = glb.size();
  out.messageZh = "Export OK.";
  // Note: Draco/Meshopt container extensions deferred; geometry already simplified in Wash.
  (void)req.compression;
  return out;
}

}  // namespace modelwash
