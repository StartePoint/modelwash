#include "modelwash/main_window.hpp"

#include <QButtonGroup>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace modelwash {

MainWindow::MainWindow(DocumentSession* session, QWidget* parent)
    : QMainWindow(parent), session_(session) {
  setWindowTitle(QStringLiteral("洗模型 — ModelWash"));
  resize(1320, 860);
  buildUi();
  applyStyle();

  connect(session_, &DocumentSession::statsChanged, this, &MainWindow::refreshUi);
  connect(session_, &DocumentSession::historyChanged, this, &MainWindow::refreshUi);
  connect(session_, &DocumentSession::personaChanged, this, &MainWindow::refreshUi);
  connect(session_, &DocumentSession::washPreviewChanged, this, &MainWindow::refreshWashUi);
  connect(session_, &DocumentSession::washTargetChanged, this, &MainWindow::refreshWashUi);
  connect(session_, &DocumentSession::documentChanged, this, [this]() {
    viewport_->setDocument(session_->hasDocument() ? &session_->document() : nullptr);
    refreshUi();
  });
  connect(session_, &DocumentSession::errorOccurred, this,
          [this](const QString& m) { showToast(m, true); });
  connect(session_, &DocumentSession::infoOccurred, this,
          [this](const QString& m) { showToast(m, false); });

  refreshUi();
}

QPushButton* MainWindow::makeSoftBtn(const QString& text, bool primary) {
  auto* b = new QPushButton(text);
  b->setCursor(Qt::PointingHandCursor);
  b->setProperty("primary", primary);
  b->setMinimumHeight(primary ? 40 : 34);
  return b;
}

void MainWindow::buildUi() {
  rootStack_ = new QStackedWidget(this);
  setCentralWidget(rootStack_);

  // ---- Onboarding ----
  onboardingPage_ = new QWidget;
  auto* obLay = new QVBoxLayout(onboardingPage_);
  obLay->setAlignment(Qt::AlignCenter);
  auto* title = new QLabel(QStringLiteral("先选一个工位身份"));
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet(QStringLiteral("font-size:28px;font-weight:700;color:#1C1F2A;"));
  auto* sub = new QLabel(QStringLiteral("本地处理 · 不上传云端"));
  sub->setAlignment(Qt::AlignCenter);
  sub->setStyleSheet(QStringLiteral("color:#8A90A2;font-size:14px;"));
  auto* row = new QHBoxLayout;
  row->setSpacing(16);
  auto* shipper = makeSoftBtn(QStringLiteral("我要出货（游戏向）"), true);
  auto* cleanup = makeSoftBtn(QStringLiteral("我要清洗（网页向）"), true);
  connect(shipper, &QPushButton::clicked, this, &MainWindow::onPersonaShipper);
  connect(cleanup, &QPushButton::clicked, this, &MainWindow::onPersonaCleanup);
  row->addWidget(shipper);
  row->addWidget(cleanup);
  obLay->addWidget(title);
  obLay->addWidget(sub);
  obLay->addSpacing(20);
  obLay->addLayout(row);
  rootStack_->addWidget(onboardingPage_);

  // ---- Workspace ----
  workspacePage_ = new QWidget;
  auto* root = new QVBoxLayout(workspacePage_);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // Top bar
  auto* top = new QFrame;
  top->setObjectName(QStringLiteral("topBar"));
  top->setFixedHeight(56);
  auto* topLay = new QHBoxLayout(top);
  topLay->setContentsMargins(16, 8, 16, 8);
  auto* brandCol = new QVBoxLayout;
  brandCol->setSpacing(0);
  auto* brand = new QLabel(QStringLiteral("洗模型 — ModelWash"));
  brand->setStyleSheet(QStringLiteral("font-weight:700;font-size:15px;color:#1C1F2A;"));
  auto* brandSub = new QLabel(QStringLiteral("本地工位 · 不上传"));
  brandSub->setStyleSheet(QStringLiteral("color:#8A90A2;font-size:11px;"));
  brandCol->addWidget(brand);
  brandCol->addWidget(brandSub);
  topLay->addLayout(brandCol);
  topLay->addStretch();
  auto* openBtn = makeSoftBtn(QStringLiteral("打开"));
  undoBtn_ = makeSoftBtn(QStringLiteral("撤销"));
  redoBtn_ = makeSoftBtn(QStringLiteral("重做"));
  exportBtn_ = makeSoftBtn(QStringLiteral("导出"), true);
  connect(openBtn, &QPushButton::clicked, this, &MainWindow::onOpen);
  connect(undoBtn_, &QPushButton::clicked, session_, &DocumentSession::undo);
  connect(redoBtn_, &QPushButton::clicked, session_, &DocumentSession::redo);
  connect(exportBtn_, &QPushButton::clicked, this, &MainWindow::onExport);
  topLay->addWidget(openBtn);
  topLay->addWidget(undoBtn_);
  topLay->addWidget(redoBtn_);
  topLay->addWidget(exportBtn_);
  root->addWidget(top);

  // Body: rail | left | viewport | right
  auto* body = new QHBoxLayout;
  body->setContentsMargins(12, 12, 12, 12);
  body->setSpacing(12);

  // Rail
  auto* rail = new QFrame;
  rail->setObjectName(QStringLiteral("rail"));
  rail->setFixedWidth(72);
  auto* railLay = new QVBoxLayout(rail);
  railLay->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  auto* washNav = makeSoftBtn(QStringLiteral("清洗"), true);
  washNav->setFixedSize(56, 56);
  connect(washNav, &QPushButton::clicked, this, &MainWindow::onModeWash);
  railLay->addWidget(washNav);
  auto* stub1 = new QLabel(QStringLiteral("修一修"));
  stub1->setAlignment(Qt::AlignCenter);
  stub1->setStyleSheet(QStringLiteral("color:#8A90A2;font-size:11px;"));
  auto* stub2 = new QLabel(QStringLiteral("拼一拼"));
  stub2->setAlignment(Qt::AlignCenter);
  stub2->setStyleSheet(QStringLiteral("color:#8A90A2;font-size:11px;"));
  auto* stub3 = new QLabel(QStringLiteral("提好看"));
  stub3->setAlignment(Qt::AlignCenter);
  stub3->setStyleSheet(QStringLiteral("color:#8A90A2;font-size:11px;"));
  railLay->addSpacing(8);
  railLay->addWidget(stub1);
  railLay->addWidget(stub2);
  railLay->addWidget(stub3);
  railLay->addStretch();
  body->addWidget(rail);

  // Left workspace card
  auto* left = new QFrame;
  left->setObjectName(QStringLiteral("card"));
  left->setFixedWidth(240);
  auto* leftLay = new QVBoxLayout(left);
  auto* leftTitle = new QLabel(QStringLiteral("工作区"));
  leftTitle->setStyleSheet(QStringLiteral("font-weight:700;color:#1C1F2A;"));
  modelInfoLabel_ = new QLabel(QStringLiteral("还没有模型"));
  modelInfoLabel_->setWordWrap(true);
  modelInfoLabel_->setStyleSheet(QStringLiteral("color:#8A90A2;"));
  auto* importBtn = makeSoftBtn(QStringLiteral("+ 导入模型"), true);
  connect(importBtn, &QPushButton::clicked, this, &MainWindow::onOpen);
  leftLay->addWidget(leftTitle);
  leftLay->addWidget(modelInfoLabel_);
  leftLay->addStretch();
  leftLay->addWidget(importBtn);
  body->addWidget(left);

  // Center viewport
  auto* center = new QFrame;
  center->setObjectName(QStringLiteral("card"));
  auto* centerLay = new QVBoxLayout(center);
  centerLay->setContentsMargins(2, 2, 2, 2);
  viewport_ = new ModelViewport;
  centerLay->addWidget(viewport_, 1);
  auto* hint = new QLabel(QStringLiteral("拖入 GLB/OBJ · 左键旋转 · 滚轮缩放 · 右键平移"));
  hint->setAlignment(Qt::AlignCenter);
  hint->setStyleSheet(QStringLiteral("color:#8A90A2;font-size:12px;padding:6px;"));
  centerLay->addWidget(hint);
  toastLabel_ = new QLabel;
  toastLabel_->setAlignment(Qt::AlignCenter);
  toastLabel_->setVisible(false);
  toastLabel_->setStyleSheet(
      QStringLiteral("background:#CCFFFFFF;border:1px solid #E6E8F0;border-radius:14px;"
                     "padding:6px 12px;color:#1C1F2A;"));
  centerLay->addWidget(toastLabel_, 0, Qt::AlignHCenter);
  body->addWidget(center, 1);

  // Right wash panel
  auto* right = new QFrame;
  right->setObjectName(QStringLiteral("card"));
  right->setFixedWidth(280);
  auto* rightLay = new QVBoxLayout(right);
  auto* rightTitle = new QLabel(QStringLiteral("清洗"));
  rightTitle->setStyleSheet(QStringLiteral("font-weight:700;font-size:16px;"));
  rightLay->addWidget(rightTitle);

  auto* targetRow = new QHBoxLayout;
  targetWeb_ = makeSoftBtn(QStringLiteral("网页"));
  targetGame_ = makeSoftBtn(QStringLiteral("游戏"));
  targetCustom_ = makeSoftBtn(QStringLiteral("自定义"));
  connect(targetWeb_, &QPushButton::clicked, this,
          [this]() { session_->setWashTarget(0); });
  connect(targetGame_, &QPushButton::clicked, this,
          [this]() { session_->setWashTarget(1); });
  connect(targetCustom_, &QPushButton::clicked, this,
          [this]() { session_->setWashTarget(2); });
  targetRow->addWidget(targetWeb_);
  targetRow->addWidget(targetGame_);
  targetRow->addWidget(targetCustom_);
  rightLay->addLayout(targetRow);

  beforeLabel_ = new QLabel(QStringLiteral("清洗前 —"));
  afterLabel_ = new QLabel(QStringLiteral("清洗后 —"));
  washMsgLabel_ = new QLabel;
  washMsgLabel_->setWordWrap(true);
  washMsgLabel_->setStyleSheet(QStringLiteral("color:#8A90A2;font-size:12px;"));
  rightLay->addWidget(beforeLabel_);
  rightLay->addWidget(afterLabel_);
  rightLay->addWidget(washMsgLabel_);

  previewWashBtn_ = makeSoftBtn(QStringLiteral("预览清洗效果"), true);
  applyWashBtn_ = makeSoftBtn(QStringLiteral("应用清洗"));
  exportGlbBtn_ = makeSoftBtn(QStringLiteral("导出 GLB"));
  connect(previewWashBtn_, &QPushButton::clicked, session_, &DocumentSession::previewWash);
  connect(applyWashBtn_, &QPushButton::clicked, session_, &DocumentSession::applyWash);
  connect(exportGlbBtn_, &QPushButton::clicked, this, &MainWindow::onExport);
  rightLay->addWidget(previewWashBtn_);
  rightLay->addWidget(applyWashBtn_);
  rightLay->addWidget(exportGlbBtn_);
  rightLay->addStretch();

  statusFaces_ = new QLabel(QStringLiteral("面数 0"));
  statusTex_ = new QLabel(QStringLiteral("贴图 ~0 KB"));
  statusWarn_ = new QLabel(QStringLiteral("状态良好"));
  statusWarn_->setStyleSheet(QStringLiteral("color:#8A90A2;"));
  rightLay->addWidget(statusFaces_);
  rightLay->addWidget(statusTex_);
  rightLay->addWidget(statusWarn_);

  body->addWidget(right);
  root->addLayout(body, 1);
  rootStack_->addWidget(workspacePage_);
}

void MainWindow::applyStyle() {
  setStyleSheet(QStringLiteral(
      "QMainWindow { background:#F2F3F7; }"
      "QFrame#topBar, QFrame#card, QFrame#rail {"
      "  background:#FFFFFF; border:1px solid #E6E8F0; border-radius:16px; }"
      "QFrame#topBar { border-radius:0; border-left:none; border-right:none; border-top:none; }"
      "QPushButton {"
      "  background:#F7F8FC; color:#1C1F2A; border:1px solid #E6E8F0;"
      "  border-radius:12px; padding:6px 14px; font-size:13px; }"
      "QPushButton:hover { background:#EEF1FF; }"
      "QPushButton:disabled { color:#B0B4C0; }"
      "QPushButton[primary=\"true\"] {"
      "  background:#4C6FFF; color:white; border:none; font-weight:600; }"
      "QPushButton[primary=\"true\"]:hover { background:#3B5CE8; }"
      "QPushButton[selected=\"true\"] {"
      "  background:#EEF1FF; color:#4C6FFF; border:1px solid #4C6FFF; }"));
}

void MainWindow::onOpen() {
  const QString path = QFileDialog::getOpenFileName(
      this, QStringLiteral("打开模型"), {},
      QStringLiteral("3D 模型 (*.glb *.gltf *.obj);;所有文件 (*)"));
  if (!path.isEmpty()) {
    session_->openFile(QUrl::fromLocalFile(path));
  }
}

void MainWindow::onExport() {
  if (!session_->hasDocument()) {
    showToast(QStringLiteral("还没有模型可导出。"), true);
    return;
  }
  const QString path = QFileDialog::getSaveFileName(
      this, QStringLiteral("导出 GLB"), QStringLiteral("model.glb"),
      QStringLiteral("GLB (*.glb)"));
  if (!path.isEmpty()) {
    session_->exportGlb(QUrl::fromLocalFile(path));
  }
}

void MainWindow::onModeWash() {
  session_->setMode(QStringLiteral("wash"));
}

void MainWindow::onPersonaShipper() {
  session_->completeOnboarding(QStringLiteral("assetShipper"));
  refreshUi();
}

void MainWindow::onPersonaCleanup() {
  session_->completeOnboarding(QStringLiteral("aiCleanup"));
  refreshUi();
}

void MainWindow::refreshUi() {
  rootStack_->setCurrentWidget(session_->needsOnboarding() ? onboardingPage_
                                                           : workspacePage_);
  undoBtn_->setEnabled(session_->canUndo());
  redoBtn_->setEnabled(session_->canRedo());
  if (session_->hasDocument()) {
    modelInfoLabel_->setText(
        QStringLiteral("当前模型\n%1 三角").arg(session_->triangleCount()));
    modelInfoLabel_->setStyleSheet(QStringLiteral("color:#1C1F2A;font-weight:600;"));
  } else {
    modelInfoLabel_->setText(QStringLiteral("还没有模型"));
    modelInfoLabel_->setStyleSheet(QStringLiteral("color:#8A90A2;"));
  }
  statusFaces_->setText(QStringLiteral("面数 %1").arg(session_->triangleCount()));
  statusTex_->setText(
      QStringLiteral("贴图 ~%1 KB").arg(session_->textureBytes() / 1024));
  const QString warn = session_->statusWarning();
  statusWarn_->setText(warn.isEmpty() ? QStringLiteral("状态良好") : warn);
  statusWarn_->setStyleSheet(warn.isEmpty() ? QStringLiteral("color:#8A90A2;")
                                            : QStringLiteral("color:#C48A1A;"));
  refreshWashUi();
}

void MainWindow::refreshWashUi() {
  const int t = session_->washTarget();
  targetWeb_->setProperty("selected", t == 0);
  targetGame_->setProperty("selected", t == 1);
  targetCustom_->setProperty("selected", t == 2);
  targetWeb_->style()->unpolish(targetWeb_);
  targetWeb_->style()->polish(targetWeb_);
  targetGame_->style()->unpolish(targetGame_);
  targetGame_->style()->polish(targetGame_);
  targetCustom_->style()->unpolish(targetCustom_);
  targetCustom_->style()->polish(targetCustom_);

  const auto before = session_->washBefore();
  const auto after = session_->washAfter();
  if (before.contains(QStringLiteral("triangles"))) {
    beforeLabel_->setText(
        QStringLiteral("清洗前 %1 面").arg(before.value(QStringLiteral("triangles")).toULongLong()));
  } else if (session_->hasDocument()) {
    beforeLabel_->setText(
        QStringLiteral("清洗前 %1 面").arg(session_->triangleCount()));
  } else {
    beforeLabel_->setText(QStringLiteral("清洗前 —"));
  }
  if (after.contains(QStringLiteral("triangles"))) {
    afterLabel_->setText(
        QStringLiteral("清洗后 %1 面").arg(after.value(QStringLiteral("triangles")).toULongLong()));
  } else {
    afterLabel_->setText(QStringLiteral("清洗后 —"));
  }
  washMsgLabel_->setText(session_->washMessage());
}

void MainWindow::showToast(const QString& msg, bool isError) {
  toastLabel_->setText(msg);
  toastLabel_->setStyleSheet(isError
      ? QStringLiteral("background:#FFEEEE;border:1px solid #D4534A;border-radius:14px;"
                       "padding:6px 12px;color:#D4534A;")
      : QStringLiteral("background:#CCFFFFFF;border:1px solid #E6E8F0;border-radius:14px;"
                       "padding:6px 12px;color:#1C1F2A;"));
  toastLabel_->setVisible(true);
  QTimer::singleShot(2800, this, [this]() { toastLabel_->setVisible(false); });
}

}  // namespace modelwash
