#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controllers/train_simulation.h"
#include "models/train_data.h"
#include "pages/constant_values_page.h"
#include "pages/electrical_parameter_page.h"
#include "pages/running_parameter_page.h"
#include "pages/track_parameter_page.h"
#include "pages/traction_effort_page.h"
#include "pages/train_consumption_page.h"
#include "pages/train_parameter_page.h"
#include "pages/train_power_page.h"
#include "pages/train_speed_page.h"
#include "pages/train_track_page.h"
#include "widgets/left_panel.h"
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

private slots:
  void navigateToPage(int pageIndex);

private:
  QWidget *menuWidget;
  QStackedWidget *stackedWidget;
  TrainSimulation *m_trainSimulation;
  TrainData m_trainData;
  MovingData m_movingData;
  MassData m_massData;
  LoadData m_loadData;
  ResistanceData m_resistanceData;
  TrainMotorData m_trainMotorData;
  EfficiencyData m_efficiencyData;
  PowerData m_powerData;
  EnergyData m_energyData;
  StationData m_stationData;
  LeftPanel *leftPanel;

  void setupLeftPanel();
  void setupPages();
};

#endif // MAINWINDOW_H
