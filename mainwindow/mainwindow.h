#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../pages/constant_values_page.h"
#include "../pages/electrical_parameter_page.h"
#include "../pages/output_page.h"
#include "../pages/running_parameter_page.h"
#include "../pages/track_parameter_page.h"
#include "../pages/traction_effort_page.h"
#include "../pages/train_parameter_page.h"
#include "../pages/train_power_page.h"
#include "../pages/train_speed_page.h"
#include "../widgets/left_panel.h"
#include <QHBoxLayout>
#include <QLabel>
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
