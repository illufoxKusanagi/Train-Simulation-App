#ifndef SIMULATION_TRACK_HANDLER_H
#define SIMULATION_TRACK_HANDLER_H

#include "core/appcontext.h"
#include <QObject>

class SimulationTrackHandler : public QObject {
  Q_OBJECT
public:
  explicit SimulationTrackHandler(AppContext &context);
  double calculateBrakingTrack();
  double calculateBrakingEmergencyTrack();
  double calculateNormalSimulationTrack();
  double calculateEmergencyNormalSimulationTrack();
  double calculateDelaySimulationTrack();
  double calculateEmergencyDelaySimulationTrack();
  double calculateSafetySimulationTrack();
  double calculateEmergencySafetySimulationTrack();
  double calculateMileage();

private:
  ConstantData *constantData;
  TrainData *trainData;
  MovingData *movingData;
  SimulationDatas *simulationDatas;
};

#endif // SIMULATION_TRACK_HANDLER_H
