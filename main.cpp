#include "mainwindow/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setStyleSheet("QWidget { background-color: white; color: black; }");
  MainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}
