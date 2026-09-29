#include "modelwash/document_session.hpp"

#include <QSettings>

namespace modelwash {

DocumentSession::DocumentSession(QObject* parent) : QObject(parent) {
  registry_.add(makeAssimpAdapter());
  QSettings settings;
  persona_ = settings.value(QStringLiteral("persona")).toString();
  if (persona_ == QStringLiteral("assetShipper")) {
    washTarget_ = WashTarget::Game;
  } else if (persona_ == QStringLiteral("aiCleanup")) {
    washTarget_ = WashTarget::Web;
  }
}

void DocumentSession::setMode(const QString& m) {
  if (mode_ == m) {
    return;
  }
  mode_ = m;
  emit modeChanged();
}

void DocumentSession::setPersona(const QString& p) {
  if (persona_ == p) {
    return;
  }
  persona_ = p;
  QSettings settings;
  settings.setValue(QStringLiteral("persona"), persona_);
  emit personaChanged();
}

void DocumentSession::completeOnboarding(const QString& persona) {
  setPersona(persona);
  if (persona == QStringLiteral("assetShipper")) {
    setWashTarget(static_cast<int>(WashTarget::Game));
  } else {
    setWashTarget(static_cast<int>(WashTarget::Web));
  }
}

void DocumentSession::setWashTarget(int t) {
  const auto nt = static_cast<WashTarget>(t);
  if (washTarget_ == nt) {
    return;
  }
  washTarget_ = nt;
  washedPreview_.reset();
  washAfter_ = {};
  washMessage_.clear();
  emit washTargetChanged();
  emit washPreviewChanged();
}

quint64 DocumentSession::triangleCount() const {
  return hasDoc_ ? doc_.meta().triangleCount : 0;
}

quint64 DocumentSession::textureBytes() const {
  return hasDoc_ ? doc_.meta().approxTextureBytes : 0;
}

QString DocumentSession::statusWarning() const {
  if (!hasDoc_) {
    return {};
  }
  for (const auto w : doc_.meta().warnings) {
    if (w == WarningCode::StrongFaceBudget) {
      return QStringLiteral("面数很高，强烈建议先清洗");
    }
    if (w == WarningCode::SoftFaceBudget) {
      return QStringLiteral("面数偏高，建议先清洗");
    }
  }
  if (doc_.meta().hasSkinning || doc_.meta().hasMorph || doc_.meta().hasAnimation) {
    return QStringLiteral("含骨骼/动画：清洗时将跳过减面");
  }
  if (doc_.meta().approxTextureBytes == 0) {
    return QStringLiteral("未读到贴图，预览可能只有底色");
  }
  return {};
}

WashSettings DocumentSession::currentWashSettings() const {
  if (washTarget_ == WashTarget::Custom) {
    return presetFor(WashTarget::Web);
  }
  return presetFor(washTarget_);
}

void DocumentSession::emitStats() {
  emit statsChanged();
}

void DocumentSession::openFile(const QUrl& url) {
  const QString local = url.isLocalFile() ? url.toLocalFile() : url.toString();
  emit progress(0.1, QStringLiteral("正在导入…"));
  const ImportResult ir =
      registry_.open(std::filesystem::path(local.toStdWString()));
  if (!ir.ok) {
    emit errorOccurred(QString::fromStdString(ir.messageZh));
    emit progress(1.0, {});
    return;
  }
  auto cmd = std::make_unique<ReplaceDocumentCommand>(ir.document);
  stack_.push(std::move(cmd), doc_);
  hasDoc_ = true;
  washedPreview_.reset();
  washBefore_ = {};
  washAfter_ = {};
  washMessage_.clear();
  emit historyChanged();
  emitStats();
  emit washPreviewChanged();
  emit documentChanged();
  emit progress(1.0, QStringLiteral("导入完成"));
  QString msg = QString::fromStdString(ir.messageZh);
  if (doc_.meta().approxTextureBytes > 0) {
    msg += QStringLiteral(" · 贴图 %1 KB")
               .arg(doc_.meta().approxTextureBytes / 1024);
  }
  emit infoOccurred(msg);
}

void DocumentSession::exportGlb(const QUrl& url) {
  if (!hasDoc_) {
    emit errorOccurred(QStringLiteral("还没有模型可导出。"));
    return;
  }
  const QString local = url.isLocalFile() ? url.toLocalFile() : url.toString();
  ExportRequest req;
  req.document = washedPreview_ ? &(*washedPreview_) : &doc_;
  req.output = std::filesystem::path(local.toStdWString());
  req.compression = currentWashSettings().geomCompression;
  emit progress(0.2, QStringLiteral("正在导出…"));
  const ExportResult er = ::modelwash::exportGlb(req);
  emit progress(1.0, {});
  if (!er.ok) {
    emit errorOccurred(QString::fromStdString(er.messageZh));
    return;
  }
  emit infoOccurred(QStringLiteral("导出成功（%1 KB）")
                        .arg(er.bytesWritten / 1024));
}

void DocumentSession::previewWash() {
  if (!hasDoc_) {
    emit errorOccurred(QStringLiteral("请先打开模型。"));
    return;
  }
  emit progress(0.2, QStringLiteral("正在清洗预览…"));
  const WashSettings settings = currentWashSettings();
  WashResult wr = runWash(doc_, settings);
  washBefore_ = {
      {QStringLiteral("triangles"), QVariant::fromValue(wr.stats.trianglesBefore)},
      {QStringLiteral("bytes"), QVariant::fromValue(wr.stats.textureBytesBefore)},
  };
  washAfter_ = {
      {QStringLiteral("triangles"), QVariant::fromValue(wr.stats.trianglesAfter)},
      {QStringLiteral("bytes"), QVariant::fromValue(wr.stats.estimatedFileBytes)},
  };
  washMessage_ = QString::fromStdString(wr.messageZh);
  washedPreview_ = std::move(wr.document);
  emit washPreviewChanged();
  emit progress(1.0, QStringLiteral("预览完成"));
}

void DocumentSession::applyWash() {
  if (!washedPreview_) {
    previewWash();
  }
  if (!washedPreview_) {
    return;
  }
  auto cmd = std::make_unique<ApplyWashResultCommand>(*washedPreview_);
  stack_.push(std::move(cmd), doc_);
  washedPreview_.reset();
  emit historyChanged();
  emitStats();
  emit documentChanged();
  emit infoOccurred(QStringLiteral("已应用清洗（可撤销）"));
}

void DocumentSession::undo() {
  if (stack_.undo(doc_)) {
    emit historyChanged();
    emitStats();
    emit documentChanged();
  }
}

void DocumentSession::redo() {
  if (stack_.redo(doc_)) {
    emit historyChanged();
    emitStats();
    emit documentChanged();
  }
}

}  // namespace modelwash
