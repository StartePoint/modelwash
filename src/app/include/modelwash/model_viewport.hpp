#pragma once

#include "modelwash/document.hpp"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPoint>
#include <QVector3D>

#include <memory>
#include <vector>

namespace modelwash {

class ModelViewport : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
 public:
  explicit ModelViewport(QWidget* parent = nullptr);
  ~ModelViewport() override;

  void setDocument(const Document* doc);
  void clearScene();
  void resetCamera();

 protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

 private:
  struct GpuMesh {
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer ibo{QOpenGLBuffer::IndexBuffer};
    int indexCount = 0;
    QVector3D baseColor{0.8f, 0.82f, 0.86f};
    float roughness = 0.5f;
    std::unique_ptr<QOpenGLTexture> texture;
    bool hasTexture = false;
  };

  void releaseGpu();
  void rebuildFromDocument();
  QImage makePreviewImage(const Texture& tex) const;

  const Document* doc_ = nullptr;
  bool glReady_ = false;
  bool dirty_ = false;

  std::unique_ptr<QOpenGLShaderProgram> program_;
  std::vector<std::unique_ptr<GpuMesh>> meshes_;

  float yaw_ = 28.f;
  float pitch_ = -18.f;
  float distance_ = 4.5f;
  QVector3D target_{0, 0, 0};
  bool hasBounds_ = false;
  QVector3D boundsMin_;
  QVector3D boundsMax_;

  bool orbiting_ = false;
  bool panning_ = false;
  QPoint lastPos_;
};

}  // namespace modelwash
