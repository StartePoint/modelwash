#include "modelwash/model_viewport.hpp"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>

#include <algorithm>
#include <cmath>

namespace modelwash {
namespace {

constexpr int kPreviewMaxEdge = 1024;

}  // namespace

ModelViewport::ModelViewport(QWidget* parent) : QOpenGLWidget(parent) {
  setMinimumSize(320, 240);
  setFocusPolicy(Qt::StrongFocus);
}

ModelViewport::~ModelViewport() {
  makeCurrent();
  releaseGpu();
  program_.reset();
  doneCurrent();
}

void ModelViewport::setDocument(const Document* doc) {
  doc_ = doc;
  dirty_ = true;
  update();
}

void ModelViewport::clearScene() {
  doc_ = nullptr;
  dirty_ = true;
  update();
}

void ModelViewport::resetCamera() {
  yaw_ = 28.f;
  pitch_ = -18.f;
  distance_ = 4.5f;
  target_ = {0, 0, 0};
  update();
}

void ModelViewport::initializeGL() {
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClearColor(0.91f, 0.925f, 0.96f, 1.f);

  program_ = std::make_unique<QOpenGLShaderProgram>();
  const char* vs = R"(#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNrm;
layout(location=2) in vec2 aUv;
uniform mat4 uMvp;
uniform mat3 uNrm;
out vec3 vNrm;
out vec2 vUv;
out vec3 vPos;
void main(){
  vNrm = normalize(uNrm * aNrm);
  vUv = aUv;
  vPos = aPos;
  gl_Position = uMvp * vec4(aPos, 1.0);
})";
  const char* fs = R"(#version 330 core
in vec3 vNrm;
in vec2 vUv;
in vec3 vPos;
uniform vec3 uColor;
uniform sampler2D uTex;
uniform int uHasTex;
uniform vec3 uLightDir;
uniform vec3 uEye;
out vec4 frag;
void main(){
  vec3 n = normalize(vNrm);
  vec3 base = uColor;
  if (uHasTex == 1) {
    base *= texture(uTex, vUv).rgb;
  }
  float ndl = max(dot(n, normalize(-uLightDir)), 0.0);
  float amb = 0.42;
  vec3 lit = base * (amb + 0.65 * ndl);
  vec3 h = normalize(normalize(-uLightDir) + normalize(uEye - vPos));
  float spec = pow(max(dot(n, h), 0.0), 32.0) * 0.12;
  frag = vec4(lit + vec3(spec), 1.0);
})";
  if (!program_->addShaderFromSourceCode(QOpenGLShader::Vertex, vs) ||
      !program_->addShaderFromSourceCode(QOpenGLShader::Fragment, fs) ||
      !program_->link()) {
    qWarning("ModelViewport shader link failed: %s",
             qPrintable(program_->log()));
  }
  glReady_ = true;
  dirty_ = true;
}

void ModelViewport::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
}

void ModelViewport::releaseGpu() {
  meshes_.clear();
}

QImage ModelViewport::makePreviewImage(const Texture& tex) const {
  if (tex.width <= 0 || tex.height <= 0 || tex.pixels.empty()) {
    return {};
  }
  const int ch = std::max(1, tex.channels);
  QImage img(tex.width, tex.height, QImage::Format_RGBA8888);
  const auto* src = tex.pixels.data();
  for (int y = 0; y < tex.height; ++y) {
    auto* dst = reinterpret_cast<uchar*>(img.scanLine(y));
    for (int x = 0; x < tex.width; ++x) {
      const int i = y * tex.width + x;
      if (ch >= 3) {
        dst[x * 4 + 0] = src[i * ch + 0];
        dst[x * 4 + 1] = src[i * ch + 1];
        dst[x * 4 + 2] = src[i * ch + 2];
        dst[x * 4 + 3] = ch >= 4 ? src[i * ch + 3] : 255;
      } else {
        const uchar g = src[i * ch];
        dst[x * 4 + 0] = g;
        dst[x * 4 + 1] = g;
        dst[x * 4 + 2] = g;
        dst[x * 4 + 3] = 255;
      }
    }
  }
  // Flip for OpenGL bottom-left UV origin of many glTF assets after flipV convention.
  img = img.mirrored(false, true);
  if (img.width() > kPreviewMaxEdge || img.height() > kPreviewMaxEdge) {
    img = img.scaled(kPreviewMaxEdge, kPreviewMaxEdge, Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);
  }
  return img;
}

void ModelViewport::rebuildFromDocument() {
  releaseGpu();
  hasBounds_ = false;
  if (!doc_) {
    return;
  }

  bool any = false;
  float minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
  for (const auto& node : doc_->nodes()) {
    if (!node.mesh || node.mesh->positions.empty()) {
      continue;
    }
    for (const auto& p : node.mesh->positions) {
      if (!any) {
        minX = maxX = p.x;
        minY = maxY = p.y;
        minZ = maxZ = p.z;
        any = true;
      } else {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        minZ = std::min(minZ, p.z);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
        maxZ = std::max(maxZ, p.z);
      }
    }
  }
  if (!any) {
    return;
  }

  const QVector3D center(0.5f * (minX + maxX), 0.5f * (minY + maxY),
                         0.5f * (minZ + maxZ));
  float maxDim = std::max({maxX - minX, maxY - minY, maxZ - minZ});
  if (maxDim < 1e-6f) {
    maxDim = 1.f;
  }
  const float scale = 2.0f / maxDim;
  boundsMin_ = QVector3D(-1, -1, -1);
  boundsMax_ = QVector3D(1, 1, 1);
  hasBounds_ = true;
  target_ = {0, 0, 0};
  distance_ = 4.5f;

  for (const auto& node : doc_->nodes()) {
    if (!node.mesh || node.mesh->positions.empty()) {
      continue;
    }
    const MeshData& mesh = *node.mesh;
    const bool hasNrm = mesh.normals.size() == mesh.positions.size();
    const bool hasUv = mesh.uvs.size() == mesh.positions.size();

    std::vector<float> verts;
    verts.reserve(mesh.positions.size() * 8);
    for (std::size_t i = 0; i < mesh.positions.size(); ++i) {
      const float x = (mesh.positions[i].x - center.x()) * scale;
      const float y = (mesh.positions[i].y - center.y()) * scale;
      const float z = (mesh.positions[i].z - center.z()) * scale;
      float nx = 0, ny = 1, nz = 0;
      if (hasNrm) {
        nx = mesh.normals[i].x;
        ny = mesh.normals[i].y;
        nz = mesh.normals[i].z;
      }
      float u = 0, v = 0;
      if (hasUv) {
        u = mesh.uvs[i].x;
        v = mesh.uvs[i].y;
      }
      verts.push_back(x);
      verts.push_back(y);
      verts.push_back(z);
      verts.push_back(nx);
      verts.push_back(ny);
      verts.push_back(nz);
      verts.push_back(u);
      verts.push_back(v);
    }

    std::vector<quint32> indices;
    if (!mesh.indices.empty()) {
      indices.assign(mesh.indices.begin(), mesh.indices.end());
    } else {
      indices.resize(mesh.positions.size());
      for (quint32 i = 0; i < indices.size(); ++i) {
        indices[i] = i;
      }
    }

    auto gpu = std::make_unique<GpuMesh>();
    gpu->vao.create();
    gpu->vao.bind();
    gpu->vbo.create();
    gpu->vbo.bind();
    gpu->vbo.allocate(verts.data(), static_cast<int>(verts.size() * sizeof(float)));
    gpu->ibo.create();
    gpu->ibo.bind();
    gpu->ibo.allocate(indices.data(),
                      static_cast<int>(indices.size() * sizeof(quint32)));
    gpu->indexCount = static_cast<int>(indices.size());

    program_->bind();
    program_->enableAttributeArray(0);
    program_->setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));
    program_->enableAttributeArray(1);
    program_->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 8 * sizeof(float));
    program_->enableAttributeArray(2);
    program_->setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(float), 2, 8 * sizeof(float));
    program_->release();
    gpu->vao.release();

    const Material* mat =
        node.materialId ? doc_->findMaterial(*node.materialId) : nullptr;
    if (mat) {
      float r = std::clamp(mat->baseColorFactor.x, 0.f, 1.f);
      float g = std::clamp(mat->baseColorFactor.y, 0.f, 1.f);
      float b = std::clamp(mat->baseColorFactor.z, 0.f, 1.f);
      const float luma = 0.2126f * r + 0.7152f * g + 0.0722f * b;
      if (mat->baseColorTexture) {
        if (const Texture* tex = doc_->findTexture(*mat->baseColorTexture)) {
          const QImage img = makePreviewImage(*tex);
          if (!img.isNull()) {
            gpu->texture = std::make_unique<QOpenGLTexture>(img);
            gpu->texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            gpu->texture->setMagnificationFilter(QOpenGLTexture::Linear);
            gpu->texture->setWrapMode(QOpenGLTexture::Repeat);
            gpu->texture->generateMipMaps();
            gpu->hasTexture = true;
            if (luma < 0.04f) {
              r = g = b = 1.f;
            }
          }
        }
      }
      if (!gpu->hasTexture && luma < 0.04f) {
        r = 0.82f;
        g = 0.84f;
        b = 0.88f;
      }
      gpu->baseColor = QVector3D(r, g, b);
      gpu->roughness = mat->roughness > 0.01f ? mat->roughness : 0.5f;
    }

    meshes_.push_back(std::move(gpu));
  }
}

void ModelViewport::paintGL() {
  if (dirty_ && glReady_) {
    rebuildFromDocument();
    dirty_ = false;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (!program_ || !program_->isLinked() || meshes_.empty()) {
    return;
  }

  const float aspect =
      height() > 0 ? static_cast<float>(width()) / static_cast<float>(height())
                   : 1.f;
  QMatrix4x4 proj;
  proj.perspective(45.f, aspect, 0.05f, 100.f);

  const float yawRad = qDegreesToRadians(yaw_);
  const float pitchRad = qDegreesToRadians(pitch_);
  const QVector3D eye(
      target_.x() + distance_ * std::cos(pitchRad) * std::sin(yawRad),
      target_.y() + distance_ * std::sin(pitchRad),
      target_.z() + distance_ * std::cos(pitchRad) * std::cos(yawRad));

  QMatrix4x4 view;
  view.lookAt(eye, target_, QVector3D(0, 1, 0));
  const QMatrix4x4 mvp = proj * view;
  QMatrix3x3 nrm = view.normalMatrix();

  program_->bind();
  program_->setUniformValue("uMvp", mvp);
  program_->setUniformValue("uNrm", nrm);
  program_->setUniformValue("uLightDir", QVector3D(-0.4f, -0.8f, -0.35f));
  program_->setUniformValue("uEye", eye);

  for (auto& mesh : meshes_) {
    program_->setUniformValue("uColor", mesh->baseColor);
    program_->setUniformValue("uHasTex", mesh->hasTexture ? 1 : 0);
    if (mesh->hasTexture && mesh->texture) {
      mesh->texture->bind(0);
      program_->setUniformValue("uTex", 0);
    }
    mesh->vao.bind();
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, nullptr);
    mesh->vao.release();
    if (mesh->hasTexture && mesh->texture) {
      mesh->texture->release();
    }
  }
  program_->release();
}

void ModelViewport::mousePressEvent(QMouseEvent* event) {
  lastPos_ = event->pos();
  if (event->button() == Qt::LeftButton) {
    orbiting_ = true;
  } else if (event->button() == Qt::MiddleButton ||
             (event->button() == Qt::RightButton)) {
    panning_ = true;
  }
}

void ModelViewport::mouseMoveEvent(QMouseEvent* event) {
  const QPoint d = event->pos() - lastPos_;
  lastPos_ = event->pos();
  if (orbiting_) {
    yaw_ += d.x() * 0.35f;
    pitch_ = std::clamp(pitch_ - d.y() * 0.35f, -89.f, 89.f);
    update();
  } else if (panning_) {
    const float sens = distance_ * 0.0015f;
    const float yawRad = qDegreesToRadians(yaw_);
    QVector3D right(std::cos(yawRad), 0, -std::sin(yawRad));
    QVector3D up(0, 1, 0);
    target_ -= right * (d.x() * sens);
    target_ += up * (d.y() * sens);
    update();
  }
}

void ModelViewport::mouseReleaseEvent(QMouseEvent* event) {
  Q_UNUSED(event);
  orbiting_ = false;
  panning_ = false;
}

void ModelViewport::wheelEvent(QWheelEvent* event) {
  const float steps = event->angleDelta().y() / 120.f;
  distance_ = std::clamp(distance_ * std::pow(0.9f, steps), 0.4f, 40.f);
  update();
}

}  // namespace modelwash
