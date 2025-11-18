export interface SimulationConfig {
  type: "static" | "dynamic";
}

export interface SimulationDataPoint {
  time: number;
  speed: number;
  speedSI: number;
  distance: number;
  acceleration: number;
  vvvfPower: number;
  catenaryPower: number;
  vvvfCurrent: number;
  catenaryCurrent: number;
  tractiveEffort: number;
  totalResistance: number;
  phase: string;
}

export interface SimulationSummary {
  maxSpeed: number;
  maxVvvfPower: number;
  maxCatenaryPower: number;
  maxVvvfCurrent: number;
  maxCatenaryCurrent: number;
  maxTractionEffort: number;
  distanceTravelled: number;
  maxEnergyConsumption: number;
  maxEnergyRegen: number;
  adhesion: number;
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
