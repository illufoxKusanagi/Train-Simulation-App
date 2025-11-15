// ============================================================
// Train Simulation API Client
// ============================================================
// Maps frontend TypeScript types to backend C++ structs
// All parameters stored in AppContext before simulation runs

const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || "http://localhost:8080";

// ============================================================
// Type Definitions (matching backend structures)
// ============================================================

export interface TrainParameters {
  numberOfMotorCars: number; // n_tm
  numberOfAxles: number; // n_axle
  numberOfCars: number; // n_car
  gearRatio: number; // gearRatio
  wheelDiameter: number; // wheel (meters)
  trainsetLength: number; // trainsetLength (meters)
  numberOfM1Cars: number; // n_M1
  numberOfM2Cars: number; // n_M2
  numberOfTcCars: number; // n_TC
  numberOfT1Cars: number; // n_T1
  numberOfT2Cars: number; // n_T2
  numberOfT3Cars: number; // n_T3
}

export interface ElectricalParameters {
  lineVoltage: number; // stat_vol_line (V)
  motorVoltage: number; // stat_vol_motor (V)
  powerFactor: number; // stat_pf (0-1)
  gearEfficiency: number; // stat_eff_gear (0-1)
  motorEfficiency: number; // stat_eff_motor (0-1)
  vvvfEfficiency: number; // stat_eff_vvvf (0-1)
  auxiliaryPower: number; // p_aps (kW)
}

export interface RunningParameters {
  // Passenger load (8 parameters)
  load: number; // load type
  massPerPassenger: number; // mass_P (kg)
  passengersPerM1: number; // n_PM1
  passengersPerM2: number; // n_PM2
  passengersPerTc: number; // n_PTc
  passengersPerT1: number; // n_PT1
  passengersPerT2: number; // n_PT2
  passengersPerT3: number; // n_PT3

  // Motion parameters (4 parameters)
  acceleration: number; // acc (km/h to m/s²)
  deceleration: number; // decc (km/h to m/s²)
  speedLimit: number; // v_limit (m/s)
  stationDistance: number; // x_station (m)
}

export interface TrackParameters {
  numberOfStations: number; // n_station
}

export interface MassParameters {
  // Car masses (kg)
  massM1: number; // mass_M1
  massM2: number; // mass_M2
  massTc: number; // mass_TC
  massT1: number; // mass_T1
  massT2: number; // mass_T2
  massT3: number; // mass_T3

  // Rotational inertia coefficients
  rotationalInertiaMotor: number; // i_M
  rotationalInertiaTrailer: number; // i_T
}

export interface SimulationConfig {
  type: "static" | "dynamic";
}

export interface SimulationResults {
  trainSpeeds: number[];
  vvvfPowers: number[];
  distances: number[];
  times: number[];
  accelerations: number[];
  // ... more result fields
}

// ============================================================
// API Client
// ============================================================

export const api = {
  // ==================== Health Check ====================
  checkHealth: async (): Promise<{ status: string; dataStatus: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/health`);
    if (!res.ok) throw new Error(`Health check failed: ${res.status}`);
    return res.json();
  },

  // ==================== Train Parameters ====================
  getTrainParameters: async (): Promise<{
    trainParameters: TrainParameters;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/train`);
    if (!res.ok)
      throw new Error(`Failed to get train parameters: ${res.status}`);
    return res.json();
  },

  updateTrainParameters: async (
    params: TrainParameters
  ): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/train`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ trainParameters: params }),
    });
    if (!res.ok)
      throw new Error(`Failed to update train parameters: ${res.status}`);
    return res.json();
  },

  // ==================== Electrical Parameters ====================
  getElectricalParameters: async (): Promise<{
    electricalParameters: ElectricalParameters;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/electrical`);
    if (!res.ok)
      throw new Error(`Failed to get electrical parameters: ${res.status}`);
    return res.json();
  },

  updateElectricalParameters: async (
    params: ElectricalParameters
  ): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/electrical`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ electricalParameters: params }),
    });
    if (!res.ok)
      throw new Error(`Failed to update electrical parameters: ${res.status}`);
    return res.json();
  },

  // ==================== Running Parameters ====================
  getRunningParameters: async (): Promise<{
    runningParameters: RunningParameters;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/running`);
    if (!res.ok)
      throw new Error(`Failed to get running parameters: ${res.status}`);
    return res.json();
  },

  updateRunningParameters: async (
    params: RunningParameters
  ): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/running`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ runningParameters: params }),
    });
    if (!res.ok)
      throw new Error(`Failed to update running parameters: ${res.status}`);
    return res.json();
  },

  // ==================== Track Parameters ====================
  getTrackParameters: async (): Promise<{
    trackParameters: TrackParameters;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/track`);
    if (!res.ok)
      throw new Error(`Failed to get track parameters: ${res.status}`);
    return res.json();
  },

  updateTrackParameters: async (
    params: TrackParameters
  ): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/track`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ trackParameters: params }),
    });
    if (!res.ok)
      throw new Error(`Failed to update track parameters: ${res.status}`);
    return res.json();
  },

  // ==================== Mass Parameters ====================
  getMassParameters: async (): Promise<{
    massParameters: MassParameters;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/mass`);
    if (!res.ok)
      throw new Error(`Failed to get mass parameters: ${res.status}`);
    return res.json();
  },

  updateMassParameters: async (
    params: MassParameters
  ): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/mass`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ massParameters: params }),
    });
    if (!res.ok)
      throw new Error(`Failed to update mass parameters: ${res.status}`);
    return res.json();
  },

  // ==================== Simulation Control ====================
  startSimulation: async (
    config: SimulationConfig
  ): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/simulation/start`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(config),
    });
    if (!res.ok) throw new Error(`Failed to start simulation: ${res.status}`);
    return res.json();
  },

  getSimulationStatus: async (): Promise<{
    status: string;
    isRunning: boolean;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/simulation/status`);
    if (!res.ok)
      throw new Error(`Failed to get simulation status: ${res.status}`);
    return res.json();
  },

  getSimulationResults: async (): Promise<{
    results: SimulationResults;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/simulation/results`);
    if (!res.ok)
      throw new Error(`Failed to get simulation results: ${res.status}`);
    return res.json();
  },
};

// import axios, { AxiosInstance } from "axios";

// const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || "http://localhost:8080";

// class TrainSimulationAPI {
//   private client: AxiosInstance;

//   constructor() {
//     this.client = axios.create({
//       baseURL: API_BASE_URL,
//       timeout: 10000,
//       headers: {
//         "Content-Type": "application/json",
//       },
//     });
//   }

//   // Health check
//   async checkHealth(): Promise<{ status: string; dataStatus: string }> {
//     const response = await this.client.get("/api/health");
//     return response.data;
//   }

//   // Add more API methods as needed
//   async getTrainParameters(): Promise<any> {
//     const response = await this.client.get("/api/parameters/train");
//     return response.data;
//   }

//   async updateTrainParameters(params: any): Promise<any> {
//     const response = await this.client.post("/api/parameters/train", params);
//     return response.data;
//   }
// }

// export const api = new TrainSimulationAPI();
