#ifndef STATIC_SIMULATION_HANDLER_H
#define STATIC_SIMULATION_HANDLER_H

#include "controllers/chart/save_button_handler.h"
#include "controllers/train_simulation/train_simulation.h"
#include "styles/colors.h"
#include <QLineSeries>
#include <QObject>
#include <QValueAxis>

class StaticSimulationChartHandler : public QObject {
  Q_OBJECT
public:
  explicit StaticSimulationChartHandler(
      TrainSimulation *trainSimulation, QChart *chart, QString &chartTitle,
      SaveButtonHandler::SimulationType *simulationType);
  void updateStaticChart();

private:
  TrainSimulation *m_trainSimulation;
  QChart *m_chart;
  QString m_chartTitle;
  SaveButtonHandler::SimulationType *m_simulationType;
  double m_maxValue;
  double m_roundedMaxValue;
  double m_minValue;
  double m_roundedMinValue;
  void setupStaticSpeedChart();
  void setupStaticTractionChart();
  void setupStaticTrackChart();
  void setupStaticPowerChart();
  void setupStaticCurrentChart();
  void setupStaticEnergyChart();
  void setupStaticAxis();
};

#endif // STATIC_SIMULATION_HANDLER_H
