#include "simulation_track_handler.h"
#include <cmath>

SimulationTrackHandler::SimulationTrackHandler(AppContext &context)
    : constantData(context.constantData.data()),
      trainData(context.trainData.data()),
      movingData(context.movingData.data()), massData(context.massData.data()),
      stationData(context.stationData.data()),
      simulationDatas(context.simulationDatas.data()) {}

double SimulationTrackHandler::calculateBrakingTrack(double speed) {
  double v_current = speed / constantData->cV;
  if (v_current <= 0) {
    return 0.0;
  }

  double v_b1 = movingData->v_b1 / constantData->cV;
  double v_b2 = movingData->v_b2 / constantData->cV;
  double decc = movingData->decc_start_si; // Deceleration in m/s²

  double totalDistance = 0.0;

  // --- CLASSIC CONSTANT DECELERATION FORMULA ---
  // Assumes deceleration is constant throughout braking, ignores field
  // weakening zones. d = v² / (2 × a)
  // totalDistance = (v_current * v_current) / (2.0 * decc);

  // --- VARIABLE DECELERATION (field weakening zones) ---
  // Deceleration varies with speed in each zone — must integrate F=ma via
  // chain
  // rule. Case 1: v <= v_b1 → decc constant → d = v² / 2a  (same as
  // classic
  // // above)
  if (v_current <= v_b1) {
    totalDistance = (v_current * v_current) / (2.0 * decc);
  }
  // // Case 2: v_b1 < v <= v_b2 → F ∝ v_b1/v → a = decc·v_b1/v
  // v·dv/dx = -decc·v_b1/v  →  v²dv = -decc·v_b1 dx
  // d_weakening  = (v_current³ - v_b1³) / (3 × decc × v_b1)
  // d_constant   = v_b1² / (2 × decc)
  else if (v_current > v_b1 && v_current < v_b2) {
    double d_weakening =
        (pow(v_current, 3) - pow(v_b1, 3)) / (3.0 * decc * v_b1);
    double d_constant = (v_b1 * v_b1) / (2.0 * decc);
    totalDistance = d_weakening + d_constant;
  }
  // Case 3: v > v_b2 → F ∝ v_b1·v_b2/v² → a = decc·v_b1·v_b2/v²
  // v·dv/dx = -decc·v_b1·v_b2/v²  →  v³dv = -decc·v_b1·v_b2 dx
  // d_weakening2 = (v_current⁴ - v_b2⁴) / (4 × decc × v_b1 × v_b2)
  // d_weakening1 = (v_b2³ - v_b1³) / (3 × decc × v_b1)
  // d_constant   = v_b1² / (2 × decc)
  else {
    double d_weakening2 =
        (pow(v_current, 4) - pow(v_b2, 4)) / (4.0 * decc * v_b1 * v_b2);
    double d_weakening1 = (pow(v_b2, 3) - pow(v_b1, 3)) / (3.0 * decc * v_b1);
    double d_constant = (v_b1 * v_b1) / (2.0 * decc);
    totalDistance = d_weakening2 + d_weakening1 + d_constant;
  }

  return totalDistance;
}

double SimulationTrackHandler::calculateStaticBrakingTrack() {
  double speed = simulationDatas->trainSpeedsSi.last();
  if (simulationDatas->trainSpeedsSi.isEmpty())
    speed = 0.0;

  double brakingDistance = 0.0;
  brakingDistance = (speed * constantData->t_reaction) +
                    pow(speed, 2) / (2.0 * movingData->decc_start_si);
  return brakingDistance;
}

double SimulationTrackHandler::calculateBrakingEmergencyTrack() {
  double speed = simulationDatas->trainSpeedsSi.last();
  if (simulationDatas->trainSpeedsSi.isEmpty())
    speed = 0.0;

  double brakingTrack = (speed * constantData->t_reaction) +
                        (pow(speed, 2) / (2 * movingData->decc_emergency_si));
  return brakingTrack;

  // double speed = simulationDatas->trainSpeedsSi.last(); // Already in m/s

  // // Reaction distance (constant speed during reaction time)
  // double reactionDistance = speed * constantData->t_reaction;

  // // Emergency braking uses higher deceleration, but still affected by field
  // // weakening Convert current speed to km/h for calculateBrakingTrack
  // double speed_kmh = speed * constantData->cV;

  // // Use the corrected braking distance calculation (which accounts for field
  // // weakening) Note: This assumes emergency decc is similar to normal decc
  // // ratio-wise For true emergency braking, we'd need separate
  // v_b1_emergency,
  // // v_b2_emergency
  // double brakingDistance = calculateBrakingTrack(speed_kmh);

  // // Scale by the ratio of emergency to normal deceleration
  // double decc_ratio = movingData->decc_emergency_si /
  // movingData->decc_start_si; double emergencyBrakingDistance =
  //     brakingDistance / movingData->decc_emergency_si;

  // return reactionDistance + emergencyBrakingDistance;
}

double SimulationTrackHandler::calculateNormalSimulationTrack() {
  if (simulationDatas->distanceTotal.isEmpty())
    return 0.0;
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateStaticBrakingTrack();
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

double SimulationTrackHandler::calculateDelaySimulationTrack() {
  if (simulationDatas->distanceTotal.isEmpty() ||
      simulationDatas->trainSpeedsSi.isEmpty() ||
      simulationDatas->accelerationsSi.isEmpty())
    return 0.0;
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateStaticBrakingTrack();
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

double SimulationTrackHandler::calculateSafetySimulationTrack() {
  return 1.2 * calculateDelaySimulationTrack();
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
