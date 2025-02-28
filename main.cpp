#include "mainwindow/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  // a.setStyleSheet("QWidget { background-color: white; color: black; }");
  // MainWindow w;
  // w.show();
  // return a.exec();
  try {
    qDebug() << "Starting application";
    QApplication app(argc, argv);

    qDebug() << "Creating main window";
    MainWindow mainWindow;

    qDebug() << "Showing main window";
    mainWindow.show();

    qDebug() << "Entering event loop";
    return app.exec();
  } catch (const std::exception &e) {
    qDebug() << "Exception caught in main:" << e.what();
    return 1;
  } catch (...) {
    qDebug() << "Unknown exception caught in main";
    return 2;
  }
}
