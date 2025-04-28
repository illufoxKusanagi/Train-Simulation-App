#ifndef SIMULATION_TRACK_HANDLER_H
#define SIMULATION_TRACK_HANDLER_H

#include "core/appcontext.h"
#include <QObject>

class SimulationTrackHandler : public QObject {
  Q_OBJECT
public:
  explicit SimulationTrackHandler(AppContext &context);
  double calculateBrakingTrack(double speedLimit);
  double calculateBrakingEmergencyTrack();
  double calculateNormalSimulationTrack(double speedLimit);
  double calculateEmergencyNormalSimulationTrack();
  double calculateDelaySimulationTrack(double speedLimit);
  double calculateEmergencyDelaySimulationTrack();
  double calculateSafetySimulationTrack(double speedLimit);
  double calculateEmergencySafetySimulationTrack();
  double calculateMileage(double speedLimit);

private:
  ConstantData *constantData;
  TrainData *trainData;
  MovingData *movingData;
  SimulationDatas *simulationDatas;
};

#endif // SIMULATION_TRACK_HANDLER_H
