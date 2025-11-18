export interface SimulationConfig {
  type: "static" | "dynamic";
}

export interface SimulationDataPoint {
  phase: string;
  time: number;
  timeTotal: number;
  distances: number;
  distancesTotal: number;
  odos: number;
  brakingDistances: number;
  slopes: number;
  radiuses: number;
  speeds: number;
  speedLimits: number;
  speedsSi: number;
  accelerations: number;
  accelerationsSi: number;
  motorForce: number;
  motorResistance: number;
  totalResistance: number;
  tractionForcePerMotor: number;
  resistancePerMotor: number;
  torque: number;
  rpm: number;
  powerWheel: number;
  powerMotorOut: number;
  powerMotorIn: number;
  vvvfPowers: number;
  vvvfCurrents: number | null;
  catenaryPowers: number;
  catenaryCurrents: number | null;
  energyConsumptions: number;
  energyPowerings: number;
  energyRegenerations: number;
  energyAps: number;
  energyCatenaries: number;
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
  maxEnergyRegen: number;
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
