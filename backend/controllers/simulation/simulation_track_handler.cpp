#include "simulation_track_handler.h"
#include <cmath>

SimulationTrackHandler::SimulationTrackHandler(AppContext &context)
    : constantData(context.constantData.data()),
      trainData(context.trainData.data()),
      movingData(context.movingData.data()), massData(context.massData.data()),
      stationData(context.stationData.data()),
      simulationDatas(context.simulationDatas.data()) {}

double SimulationTrackHandler::calculateBrakingTrack(double speed) {
  // Convert speed to m/s
  double v_current_si = speed / constantData->cV;

  // If speed is zero or negative, no braking distance needed
  if (v_current_si <= 0) {
    return 0.0;
  }

  // Numerical integration matching actual braking simulation
  double v_si = v_current_si; // Current velocity (m/s)
  double v_kmh = speed;       // Current velocity (km/h) for field weakening
  double distance = 0.0;
  double dt = constantData->dt;

  // Field weakening breakpoints (km/h)
  double v_b1 = movingData->v_b1;
  double v_b2 = movingData->v_b2;
  double f_brake = massData->mass_totalInertial * movingData->decc_start;

  // Integrate until velocity reaches zero
  while (v_si > 0.01) {
    // Calculate field-weakened motor force (matches actual simulation exactly)
    double f_motor;
    if (v_kmh < v_b1) {
      // Constant Torque: F_motor = -F_brake
      f_motor = -f_brake;
    } else if (v_kmh > v_b1 && v_kmh <= v_b2) {
      // Constant Power: F_motor = -(F_brake × v_b1)/v
      f_motor = -((f_brake * v_b1) / v_kmh);
    } else if (v_kmh > v_b2) {
      // Characteristic Area: F_motor = -(F_brake × v_b1 × v_b2)/v²
      f_motor = -((f_brake * v_b1 * v_b2) / (v_kmh * v_kmh));
    } else {
      // Edge case: v == v_b1, use constant torque
      f_motor = -f_brake;
    }

    // Calculate deceleration (simplified model - motor force only)
    double decc_si = f_motor / massData->mass_totalInertial;
    double decc_kmh = decc_si * constantData->cV;

    // Distance calculation: d = v×dt + 0.5×a×dt²
    distance += v_si * dt + 0.5 * decc_si * dt * dt;

    // Update velocity
    v_si += decc_si * dt;
    v_kmh += decc_kmh * dt;

    // Safety check
    if (distance > 10000.0) {
      break;
    }
  }

  return distance;
}

double SimulationTrackHandler::calculateBrakingEmergencyTrack() {
  if (simulationDatas->trainSpeedsSi.isEmpty())
    return 0.0;

  double v_current = simulationDatas->trainSpeedsSi.last(); // m/s

  // Reaction distance (constant speed during reaction time)
  double reactionDistance = v_current * constantData->t_reaction;

  // Simple emergency braking: d = v² / (2a_emergency)
  double emergencyBrakingDistance =
      (v_current * v_current) / (2.0 * movingData->decc_emergency);

  return reactionDistance + emergencyBrakingDistance;
}

double SimulationTrackHandler::calculateNormalSimulationTrack(double speed) {
  if (simulationDatas->distanceTotal.isEmpty())
    return 0.0;
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingTrack(speed);
  return poweringDistance + trainLength + brakingDistance;
}

double SimulationTrackHandler::calculateEmergencyNormalSimulationTrack() {
  if (simulationDatas->distanceTotal.isEmpty())
    return 0.0;
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingEmergencyTrack();
  return poweringDistance + trainLength + brakingDistance;
}

double SimulationTrackHandler::calculateDelaySimulationTrack(double speed) {
  if (simulationDatas->distanceTotal.isEmpty() ||
      simulationDatas->trainSpeedsSi.isEmpty() ||
      simulationDatas->accelerationsSi.isEmpty())
    return 0.0;
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingTrack(speed);
  return poweringDistance + brakingDistance + trainLength +
         (simulationDatas->trainSpeedsSi.last() * constantData->t_delay) +
         (0.5 * simulationDatas->accelerationsSi.last() *
          pow(constantData->t_delay, 2));
}

double SimulationTrackHandler::calculateEmergencyDelaySimulationTrack() {
  if (simulationDatas->distanceTotal.isEmpty() ||
      simulationDatas->trainSpeedsSi.isEmpty() ||
      simulationDatas->accelerationsSi.isEmpty())
    return 0.0;
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingEmergencyTrack();
  return poweringDistance + brakingDistance + trainLength +
         (simulationDatas->trainSpeedsSi.last() * constantData->t_delay) +
         (0.5 * simulationDatas->accelerationsSi.last() *
          pow(constantData->t_delay, 2));
}

double SimulationTrackHandler::calculateSafetySimulationTrack(double speed) {
  return 1.2 * calculateDelaySimulationTrack(speed);
}

double SimulationTrackHandler::calculateEmergencySafetySimulationTrack() {
  return 1.2 * calculateEmergencyDelaySimulationTrack();
}

double SimulationTrackHandler::calculateOdo(double speed) {
  double distance =
      simulationDatas->odos.isEmpty() ? 0 : simulationDatas->odos.last();
  double brakingDistance = calculateBrakingTrack(speed);
  double calculatedDistance = simulationDatas->distance.isEmpty()
                                  ? 0
                                  : simulationDatas->distance.last();
  return distance + brakingDistance + calculatedDistance;
}
