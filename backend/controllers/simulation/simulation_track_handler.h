#ifndef SIMULATION_TRACK_HANDLER_H
#define SIMULATION_TRACK_HANDLER_H

#include "core/appcontext.h"
#include <QObject>

class SimulationTrackHandler : public QObject {
  Q_OBJECT
public:
  explicit SimulationTrackHandler(AppContext &context);
  double calculateBrakingTrack(double speed);
  double calculateBrakingEmergencyTrack();
  double calculateNormalSimulationTrack();
  double calculateEmergencyNormalSimulationTrack();
  double calculateDelaySimulationTrack();
  double calculateEmergencyDelaySimulationTrack();
  double calculateSafetySimulationTrack();
  double calculateEmergencySafetySimulationTrack();
  double calculateStaticBrakingTrack();
  double calculateOdo(double speed);

private:
  ConstantData *constantData;
  TrainData *trainData;
  MovingData *movingData;
  MassData *massData;
  StationData *stationData;
  SimulationDatas *simulationDatas;
};

#endif // SIMULATION_TRACK_HANDLER_H
