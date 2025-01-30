#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../widgets/left_panel.h"
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
  QWidget *menuWidget;
  QStackedWidget *stackedWidget;

  void setupLeftPanel();
  void setupPages();

  // void constantValuesPage();
  // void trainParameterPage();
  // void runningParameterPage();
  // void trackParameterPage();
  // void electricalParameterPage();

  LeftPanel *leftPanel;
};

#endif // MAINWINDOW_H
