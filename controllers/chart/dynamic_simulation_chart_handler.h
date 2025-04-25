#ifndef DYNAMIC_SIMULATION_CHART_HANDLER_H
#define DYNAMIC_SIMULATION_CHART_HANDLER_H

#include "controllers/chart/save_button_handler.h"
#include "controllers/train_simulation/train_simulation.h"
#include "styles/colors.h"
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>

class DynamicSimulationChartHandler {
public:
  explicit DynamicSimulationChartHandler(
      TrainSimulation *trainSimulation, QChart *chart, QString &chartTitle,
      SaveButtonHandler::SimulationType *simulationType);
  void updateChart();

private:
  void setupDynamicSpeedChart();
  void setupDynamicTractionChart();
  void setupDynamicTrackChart();
  void setupDynamicPowerChart();
  void setupDynamicCurrentChart();
  void setupDynamicEnergyChart();
  void setupDistanceChart();
  void setupDynamicAxis();

  TrainSimulation *m_trainSimulation;
  QChart *m_chart;
  QString m_chartTitle;
  SaveButtonHandler::SimulationType *m_simulationType;
  double m_maxValue;
  double m_roundedMaxValue;
};

#endif // DYNAMIC_SIMULATION_CHART_HANDLER_H
