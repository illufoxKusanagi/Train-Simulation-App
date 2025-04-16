#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

#include "models/simulation_data.h"
#include "models/train_data.h"
#include "utility_handler.h"
#include "widgets/message_box_widget.h"
#include <QFileDialog>
#include <QObject>
#include <fstream>
#include <iostream>

class OutputHandler : public QObject {
public:
  explicit OutputHandler(SimulationDatas &simulationDatas,
                         UtilityHandler &utilityHandler);
  void printSimulationDatas();
  bool saveTrainSpeedData();
  bool saveTractionEffortData();
  bool saveTrainPowerData();
  bool saveTrainTrackData();
  bool saveEnergyConsumptionData();
  bool saveEnergyPoweringData();
  bool saveEnergyRegenData();
  bool saveEnergyApsData();

  double findMaxSpeed();
  double findMaxVvvfPower();
  double findMaxCatenaryPower();
  double findMaxVvvfCurrent();
  double findMaxCatenaryCurrent();
  double findMaxTractionEffort();
  double findDistanceTravelled();
  double findMaxEnergyConsumption();
  double findMaxEnergyRegen();
  double findMaxEnergyPowering();
  double findMaxEnergyAps();
  double findMaxCurrTime();
  double findMaxPowTime();
  double getAdhesion();

  int getAllDataNumber();
  int getTrainSpeedDataNumber();
  int getTractionEffortDataNumber();
  int getTrainPowerDataNumber();
  int getTrainTrackDataNumber();

private:
  SimulationDatas *simulationDatas;
  UtilityHandler *utilityHandler;
};

#endif // OUTPUT_HANDLER_H
