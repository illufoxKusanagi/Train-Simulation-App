#include "output_table_handler.h"

OutputTableHandler::OutputTableHandler(TrainSimulation *trainSimulation,
                                       QVBoxLayout *layout, QString chartTitle)
    : m_trainSimulation(trainSimulation), mainLayout(layout),
      m_chartTitle(chartTitle) {}

void OutputTableHandler::setupTable() {
  QStringList dummyHeaders = {"Track distance", "Track distance on EB"};
  m_table = new QTableWidget();
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setColumnCount(dummyHeaders.size());
  m_table->setHorizontalHeaderLabels(dummyHeaders);
  m_table->setRowCount(0);
  m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_table->setStyleSheet(
      "QTableView {"
      "color : " +
      Colors::StandardBlack.name() + ";" + TextStyle::BodyMediumRegular() +
      "border: 1px solid " + Colors::Grey800.name() +
      "; "
      "}"
      "QTableView::item:selected{"
      "background-color: " +
      Colors::Primary300.name() +
      ";"
      "color: " +
      Colors::StandardBlack.name() +
      ";"
      "}"
      "QTableView::item:hover {"
      "background-color: " +
      Colors::Primary100.name() +
      ";"
      "}"
      "QHeaderView::section { background-color:" +
      Colors::Secondary400.name() + ";" + TextStyle::BodyMediumBold() +
      "color: white; padding: 6px; }");
  mainLayout->addWidget(m_table);
}

void OutputTableHandler::updateTable() {
  double normalTrackLength = m_trainSimulation->m_simulationTrackHandler
                                 ->calculateNormalSimulationTrack();
  double delayTrackLength = m_trainSimulation->m_simulationTrackHandler
                                ->calculateDelaySimulationTrack();
  double safetyTrackLength = m_trainSimulation->m_simulationTrackHandler
                                 ->calculateSafetySimulationTrack();
  double normalEmergencyTrackLength =
      m_trainSimulation->m_simulationTrackHandler
          ->calculateEmergencyNormalSimulationTrack();
  double delayEmergencyTrackLength =
      m_trainSimulation->m_simulationTrackHandler
          ->calculateEmergencyDelaySimulationTrack();
  double safetyEmergencyTrackLength =
      m_trainSimulation->m_simulationTrackHandler
          ->calculateEmergencySafetySimulationTrack();
  QList<double> normalBraking = {normalTrackLength, delayTrackLength,
                                 safetyTrackLength};
  QList<double> emergencyBraking = {normalEmergencyTrackLength,
                                    delayEmergencyTrackLength,
                                    safetyEmergencyTrackLength};
  m_table->setRowCount(normalBraking.size());
  m_table->setVerticalHeaderLabels({"Normal", "Delay 3s", "Safety 20%"});
  for (int i = 0; i < normalBraking.size(); i++)
    m_table->setItem(i, 0,
                     new QTableWidgetItem(QString::number(normalBraking[i])));
  for (int i = 0; i < emergencyBraking.size(); i++)
    m_table->setItem(
        i, 1, new QTableWidgetItem(QString::number(emergencyBraking[i])));
}
