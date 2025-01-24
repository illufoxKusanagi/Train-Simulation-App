#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../widgets/leftPanel.h"
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void navigateToPage(int pageIndex);

private:
  QWidget *menuWidget;           // Panel untuk menu di kiri
  QStackedWidget *stackedWidget; // Widget untuk menyimpan halaman

  void setupLeftPanel(); // Mengatur LeftPanel
  void setupPages();     // Mengatur halaman di QStackedWidget
  // QStackedWidget *stackedWidget;

  // void leftPanel();
  // void constantValuesPage();
  // void trainParameterPage();
  // void runningParameterPage();
  // void trackParameterPage();
  // void electricalParameterPage();

  LeftPanel *leftPanel; // Sidebar panel
};

#endif // MAINWINDOW_H
