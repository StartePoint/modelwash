#pragma once

#include "modelwash/command.hpp"
#include "modelwash/document.hpp"
#include "modelwash/io.hpp"
#include "modelwash/wash.hpp"

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantMap>

#include <optional>

namespace modelwash {

class DocumentSession : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)
  Q_PROPERTY(bool canUndo READ canUndo NOTIFY historyChanged)
  Q_PROPERTY(bool canRedo READ canRedo NOTIFY historyChanged)
  Q_PROPERTY(quint64 triangleCount READ triangleCount NOTIFY statsChanged)
  Q_PROPERTY(quint64 textureBytes READ textureBytes NOTIFY statsChanged)
  Q_PROPERTY(QString statusWarning READ statusWarning NOTIFY statsChanged)
  Q_PROPERTY(bool hasDocument READ hasDocument NOTIFY statsChanged)
  Q_PROPERTY(QString persona READ persona WRITE setPersona NOTIFY personaChanged)
  Q_PROPERTY(bool needsOnboarding READ needsOnboarding NOTIFY personaChanged)
  Q_PROPERTY(QVariantMap washBefore READ washBefore NOTIFY washPreviewChanged)
  Q_PROPERTY(QVariantMap washAfter READ washAfter NOTIFY washPreviewChanged)
  Q_PROPERTY(QString washMessage READ washMessage NOTIFY washPreviewChanged)
  Q_PROPERTY(int washTarget READ washTarget WRITE setWashTarget NOTIFY washTargetChanged)

 public:
  explicit DocumentSession(QObject* parent = nullptr);

  QString mode() const { return mode_; }
  void setMode(const QString& m);
  bool canUndo() const { return stack_.canUndo(); }
  bool canRedo() const { return stack_.canRedo(); }
  quint64 triangleCount() const;
  quint64 textureBytes() const;
  QString statusWarning() const;
  bool hasDocument() const { return hasDoc_; }
  QString persona() const { return persona_; }
  void setPersona(const QString& p);
  bool needsOnboarding() const { return persona_.isEmpty(); }
  QVariantMap washBefore() const { return washBefore_; }
  QVariantMap washAfter() const { return washAfter_; }
  QString washMessage() const { return washMessage_; }
  int washTarget() const { return static_cast<int>(washTarget_); }
  void setWashTarget(int t);

  const Document& document() const { return doc_; }

 public slots:
  void openFile(const QUrl& url);
  void exportGlb(const QUrl& url);
  void previewWash();
  void applyWash();
  void undo();
  void redo();
  void completeOnboarding(const QString& persona);

 signals:
  void modeChanged();
  void historyChanged();
  void statsChanged();
  void documentChanged();
  void personaChanged();
  void washPreviewChanged();
  void washTargetChanged();
  void errorOccurred(const QString& messageZh);
  void infoOccurred(const QString& messageZh);
  void progress(double value, const QString& messageZh);

 private:
  void emitStats();
  WashSettings currentWashSettings() const;

  Document doc_;
  CommandStack stack_;
  FormatRegistry registry_;
  QString mode_ = QStringLiteral("wash");
  bool hasDoc_ = false;
  QString persona_;
  WashTarget washTarget_ = WashTarget::Web;
  QVariantMap washBefore_;
  QVariantMap washAfter_;
  QString washMessage_;
  std::optional<Document> washedPreview_;
};

}  // namespace modelwash
