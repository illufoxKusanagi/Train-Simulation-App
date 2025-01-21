#include "mainwindow/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QWidget { background-color: white; color: black; }");
    MainWindow w;
    w.show();
    return a.exec();
}
