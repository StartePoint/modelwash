#pragma once

#include "modelwash/document_session.hpp"
#include "modelwash/model_viewport.hpp"

#include <QMainWindow>

class QLabel;
class QPushButton;
class QStackedWidget;
class QFrame;

namespace modelwash {

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(DocumentSession* session, QWidget* parent = nullptr);

 private slots:
  void onOpen();
  void onExport();
  void onModeWash();
  void onPersonaShipper();
  void onPersonaCleanup();
  void refreshUi();
  void refreshWashUi();
  void showToast(const QString& msg, bool isError);

 private:
  void buildUi();
  void applyStyle();
  QPushButton* makeSoftBtn(const QString& text, bool primary = false);

  DocumentSession* session_ = nullptr;
  ModelViewport* viewport_ = nullptr;
  QStackedWidget* rootStack_ = nullptr;

  QWidget* onboardingPage_ = nullptr;
  QWidget* workspacePage_ = nullptr;

  QLabel* modelInfoLabel_ = nullptr;
  QLabel* beforeLabel_ = nullptr;
  QLabel* afterLabel_ = nullptr;
  QLabel* washMsgLabel_ = nullptr;
  QLabel* statusFaces_ = nullptr;
  QLabel* statusTex_ = nullptr;
  QLabel* statusWarn_ = nullptr;
  QLabel* toastLabel_ = nullptr;

  QPushButton* undoBtn_ = nullptr;
  QPushButton* redoBtn_ = nullptr;
  QPushButton* exportBtn_ = nullptr;
  QPushButton* previewWashBtn_ = nullptr;
  QPushButton* applyWashBtn_ = nullptr;
  QPushButton* exportGlbBtn_ = nullptr;
  QPushButton* targetWeb_ = nullptr;
  QPushButton* targetGame_ = nullptr;
  QPushButton* targetCustom_ = nullptr;
};

}  // namespace modelwash
