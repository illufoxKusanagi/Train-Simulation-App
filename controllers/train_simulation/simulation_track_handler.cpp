#include "simulation_track_handler.h"

SimulationTrackHandler::SimulationTrackHandler(AppContext &context)
    : constantData(context.constantData.data()),
      trainData(context.trainData.data()),
      movingData(context.movingData.data()),
      simulationDatas(context.simulationData.data()) {}

double SimulationTrackHandler::calculateBrakingTrack(double speedLimit) {
  double speed = speedLimit / constantData->cV;
  double brakingTrack = (speed * constantData->t_reaction) +
                        (pow(speed, 2) / (2 * movingData->decc_start));
  return brakingTrack;
}

double SimulationTrackHandler::calculateBrakingEmergencyTrack() {
  double speed = simulationDatas->trainSpeedsSi.last();
  double brakingTrack = (speed * constantData->t_reaction) +
                        (pow(speed, 2) / (2 * movingData->decc_emergency));
  return brakingTrack;
}

double
SimulationTrackHandler::calculateNormalSimulationTrack(double speedLimit) {
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingTrack(speedLimit);
  return poweringDistance + trainLength + brakingDistance;
}

double SimulationTrackHandler::calculateEmergencyNormalSimulationTrack() {
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingEmergencyTrack();
  return poweringDistance + trainLength + brakingDistance;
}

double
SimulationTrackHandler::calculateDelaySimulationTrack(double speedLimit) {
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingTrack(speedLimit);
  return poweringDistance + brakingDistance + trainLength +
         (simulationDatas->trainSpeedsSi.last() * constantData->t_delay) +
         (0.5 * simulationDatas->accelerationsSi.last() *
          pow(constantData->t_delay, 2));
}

double SimulationTrackHandler::calculateEmergencyDelaySimulationTrack() {
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingEmergencyTrack();
  return poweringDistance + brakingDistance + trainLength +
         (simulationDatas->trainSpeedsSi.last() * constantData->t_delay) +
         (0.5 * simulationDatas->accelerationsSi.last() *
          pow(constantData->t_delay, 2));
}

double
SimulationTrackHandler::calculateSafetySimulationTrack(double speedLimit) {
  return 1.2 * calculateDelaySimulationTrack(speedLimit);
}

double SimulationTrackHandler::calculateEmergencySafetySimulationTrack() {
  return 1.2 * calculateEmergencyDelaySimulationTrack();
}

double SimulationTrackHandler::calculateMileage(double speedLimit) {
  double distance = simulationDatas->distanceTotal.isEmpty()
                        ? 0
                        : simulationDatas->distanceTotal.last();
  double brakingDistance = calculateBrakingTrack(speedLimit);
  return distance + brakingDistance;
}
