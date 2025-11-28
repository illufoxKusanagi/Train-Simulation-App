export interface SimulationConfig {
  type: "static" | "dynamic";
}

export interface SimulationDataPoint {
  // EXACTLY matching the C++ backend API response field names from simulation_handler.cpp
  phase: string; // simulationDatas->phase[i]
  iteration?: number; // i + 1 (added by frontend)
  time: number; // simulationDatas->time[i] - Time (s)
  timeTotal: number; // simulationDatas->timeTotal[i] - Total time (s)
  distances: number; // simulationDatas->distance[i] - Distance (m) - API returns "distances"
  distancesTotal: number; // simulationDatas->distanceTotal[i] - TotalDistance (m) - API returns "distancesTotal"
  odos: number; // simulationDatas->odos[i] - Odo (m)
  brakingDistances: number; // simulationDatas->brakingDistances[i] - Braking Distance
  slopes: number; // simulationDatas->slopes[i] - Slope
  radiuses: number; // simulationDatas->radiuses[i] - Radius
  speeds: number; // simulationDatas->trainSpeeds[i] - Speed (km/h) - API returns "speeds"
  speedLimits: number; // simulationDatas->speedLimits[i] - Speed Limit(km/h)
  speedsSi: number; // simulationDatas->trainSpeedsSi[i] - Speed (m/s) - API returns "speedsSi"
  accelerations: number; // simulationDatas->accelerations[i] - Acceleration (km/h/s)
  accelerationsSi: number; // simulationDatas->accelerationsSi[i] - Acceleration (m/s2)
  motorForce: number; // simulationDatas->motorForce[i] - F Motor
  motorResistance: number; // simulationDatas->motorResistance[i] - F Res
  totalResistance: number; // simulationDatas->totalResistance[i] - F Total
  tractionForcePerMotor: number; // simulationDatas->tractionForcePerMotor[i] - F Motor /TM
  resistancePerMotor: number; // simulationDatas->resistancePerMotor[i] - F Res / TM
  torque: number; // simulationDatas->torque[i] - Torque
  rpm: number; // simulationDatas->rpm[i] - RPM
  powerWheel: number; // simulationDatas->powerWheel[i] - P Wheel
  powerMotorOut: number; // simulationDatas->powerMotorOut[i] - P_motor Out
  powerMotorIn: number; // simulationDatas->powerMotorIn[i] - P_motor In
  vvvfPowers: number; // simulationDatas->vvvfPowers[i] - P_vvvf
  catenaryPowers: number; // simulationDatas->catenaryPowers[i] - P_catenary
  catenaryCurrents: number; // simulationDatas->catenaryCurrents[i] - Catenary current
  vvvfCurrents: number; // simulationDatas->vvvfCurrents[i] - VVVF current
  energyConsumptions: number; // simulationDatas->energyConsumptions[i] - Energy Consumption
  energyPowerings: number; // simulationDatas->energyPowerings[i] - Energy of Powering
  // energyRegenerations: number; // simulationDatas->energyRegenerations[i] - Energy Regen
  energyAps: number; // simulationDatas->energyAps[i] - Energy of APS
  energyCatenaries: number; // simulationDatas->energyCatenaries[i] - Energy Catenary
  motorResistancesZero: number; // simulationDatas->motorResistancesZero[i] - Run res at 0
  motorResistancesFive: number; // simulationDatas->motorResistancesFive[i] - Run res at 5
  motorResistancesTen: number; // simulationDatas->motorResistancesTen[i] - Run res at 10
  motorResistancesTwentyFive: number; // simulationDatas->motorResistancesTwentyFive[i] - Run res at 25
}

export interface SimulationSummary {
  maxSpeed: number | null;
  distanceTravelled: number | null;
  distanceOnBraking?: number;
  distanceOnEmergencyBraking?: number;
  maxTractionEffort: number | null;
  adhesion: number | null;
  maxCatenaryPower: number | null;
  maxVvvfPower: number | null;
  maxCatenaryCurrent: number | null;
  maxVvvfCurrent: number | null;
  maxCurrentTime: number;
  maxPowerTime: number;
  maxEnergyConsumption: number | null;
  maxEnergyPowering: number | null;
  // maxEnergyRegen: number;
  maxEnergyAps: number;
}

export interface TrackDistanceTable {
  normalBraking: number[];
  emergencyBraking: number[];
  labels: string[];
}

export interface SimulationResults {
  status: string;
  results: SimulationDataPoint[];
  totalPoints: number;
  returnedPoints: number;
  summary: SimulationSummary;
  trackDistanceTable?: TrackDistanceTable;
}
