#include <QApplication>
#include <QIcon>

#include "modelwash/document_session.hpp"
#include "modelwash/main_window.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName(QStringLiteral("ModelWash"));
  QCoreApplication::setApplicationName(QStringLiteral("ModelWash"));
  QCoreApplication::setApplicationVersion(QStringLiteral("0.1.0"));
  app.setWindowIcon(QIcon(QStringLiteral(":/icons/modelwash.png")));

  modelwash::DocumentSession session;
  modelwash::MainWindow window(&session);
  window.show();
  return app.exec();
}
