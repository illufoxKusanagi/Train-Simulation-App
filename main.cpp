#include "core/appcontext.h"
#include "mainwindow/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("PT INKA Persero");
  QCoreApplication::setApplicationName("Train Simulation App");
  app.setStyleSheet("QWidget { background-color: white; color: black; }");
  AppContext context;
  MainWindow mainWindow(context);
  mainWindow.show();
  return app.exec();
}
