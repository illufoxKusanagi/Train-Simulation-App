#include "simulation_track_handler.h"

SimulationTrackHandler::SimulationTrackHandler(AppContext &context)
    : constantData(context.constantData.data()),
      trainData(context.trainData.data()),
      movingData(context.movingData.data()),
      simulationDatas(context.simulationDatas.data()) {}

double SimulationTrackHandler::calculateBrakingTrack(double speed) {
  // Convert speed to m/s
  double v_current = speed / constantData->cV;
  
  // If speed is zero or negative, no braking distance needed
  if (v_current <= 0) {
    return 0.0;
  }
  
  // Get weakening points (convert from km/h to m/s)
  double v_b1 = movingData->v_b1 / constantData->cV;
  double v_b2 = movingData->v_b2 / constantData->cV;
  double decc = movingData->decc_start;  // m/s²
  
  double totalDistance = 0.0;
  
  // Case 1: Current speed is below first weakening point
  // Braking force is constant, use simple formula
  if (v_current <= v_b1) {
    totalDistance = (v_current * v_current) / (2.0 * decc);
  }
  // Case 2: Current speed is between v_b1 and v_b2
  // Braking force = f_brake × (v_b1 / v), need to integrate
  else if (v_current > v_b1 && v_current <= v_b2) {
    // Distance from v_current to v_b1 (field weakening region)
    // Using integration: ∫(v/a) dv where a = decc × (v_b1/v)
    // Result: (v² - v_b1²) / (2 × decc)
    double d_weakening = (v_current * v_current - v_b1 * v_b1) / (2.0 * decc);
    
    // Distance from v_b1 to 0 (constant deceleration)
    double d_constant = (v_b1 * v_b1) / (2.0 * decc);
    
    totalDistance = d_weakening + d_constant;
  }
  // Case 3: Current speed is above v_b2
  // Braking force = f_brake × (v_b1 × v_b2 / v²), need to integrate
  else {
    // Distance from v_current to v_b2 (second weakening region)
    // Using integration: ∫(v/a) dv where a = decc × (v_b1 × v_b2 / v²)
    // Result: (v³ - v_b2³) / (3 × decc × v_b1 × v_b2)
    double d_weakening2 = (pow(v_current, 3) - pow(v_b2, 3)) / 
                          (3.0 * decc * v_b1 * v_b2);
    
    // Distance from v_b2 to v_b1 (first weakening region)
    double d_weakening1 = (v_b2 * v_b2 - v_b1 * v_b1) / (2.0 * decc);
    
    // Distance from v_b1 to 0 (constant deceleration)
    double d_constant = (v_b1 * v_b1) / (2.0 * decc);
    
    totalDistance = d_weakening2 + d_weakening1 + d_constant;
  }
  
  return totalDistance;
}

double SimulationTrackHandler::calculateBrakingEmergencyTrack() {
  if (simulationDatas->trainSpeedsSi.isEmpty())
    return 0.0;
    
  double speed = simulationDatas->trainSpeedsSi.last();  // Already in m/s
  
  // Reaction distance (constant speed during reaction time)
  double reactionDistance = speed * constantData->t_reaction;
  
  // Emergency braking uses higher deceleration, but still affected by field weakening
  // Convert current speed to km/h for calculateBrakingTrack
  double speed_kmh = speed * constantData->cV;
  
  // Use the corrected braking distance calculation (which accounts for field weakening)
  // Note: This assumes emergency decc is similar to normal decc ratio-wise
  // For true emergency braking, we'd need separate v_b1_emergency, v_b2_emergency
  double brakingDistance = calculateBrakingTrack(speed_kmh);
  
  // Scale by the ratio of emergency to normal deceleration
  double decc_ratio = movingData->decc_emergency / movingData->decc_start;
  double emergencyBrakingDistance = brakingDistance / decc_ratio;
  
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
