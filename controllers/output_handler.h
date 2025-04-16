#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

#include "models/train_data.h"
#include <QObject>

class OutputHandler : public QObject {
public:
  explicit OutputHandler();
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
};

#endif // OUTPUT_HANDLER_H
