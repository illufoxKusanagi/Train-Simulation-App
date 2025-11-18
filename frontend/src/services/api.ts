// ============================================================
// Train Simulation API Client
// ============================================================
// Maps frontend TypeScript types to backend C++ structs
// All parameters stored in AppContext before simulation runs

// ============================================================
// Train Simulation API Client
// Uses ONLY the types declared in input-params.ts
// ============================================================

import {
  TrainConstantParams,
  TrainMassParams,
  TrainPassangerParams,
  TrainNumberParams,
  TrackParams,
  RunningParams,
  ElectricalParams,
} from "@/types/input-params";
import { SimulationConfig, SimulationResults } from "@/types/simulation-params";

const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || "http://localhost:8080";

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

  // ==================== Quick Initialization ====================
  quickInit: async (): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/init/quick`, {
      method: "POST",
    });
    if (!res.ok) throw new Error(`Quick init failed: ${res.status}`);
    return res.json();
  },

  // ==================== Debug Context ====================
  debugContext: async (): Promise<Record<string, unknown>> => {
    const res = await fetch(`${API_BASE_URL}/api/debug/context`);
    if (!res.ok) throw new Error(`Debug context failed: ${res.status}`);
    return res.json();
  },

  // ==================== Train Parameters ====================
  getTrainParameters: async (): Promise<{
    trainParameters: TrainConstantParams & TrainNumberParams;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/train`);
    if (!res.ok)
      throw new Error(`Failed to get train parameters: ${res.status}`);
    return res.json();
  },

  updateTrainParameters: async (
    params: Record<string, number>
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
    electricalParameters: ElectricalParams;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/electrical`);
    if (!res.ok)
      throw new Error(`Failed to get electrical parameters: ${res.status}`);
    return res.json();
  },

  updateElectricalParameters: async (
    params: ElectricalParams
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
    runningParameters: RunningParams & TrainPassangerParams;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/running`);
    if (!res.ok)
      throw new Error(`Failed to get running parameters: ${res.status}`);
    return res.json();
  },

  updateRunningParameters: async (
    params: Record<string, number>
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
    trackParameters: TrackParams;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/track`);
    if (!res.ok)
      throw new Error(`Failed to get track parameters: ${res.status}`);
    return res.json();
  },

  updateTrackParameters: async (
    params: TrackParams | Record<string, number | number[]>
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
    massParameters: TrainMassParams;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/mass`);
    if (!res.ok)
      throw new Error(`Failed to get mass parameters: ${res.status}`);
    return res.json();
  },

  updateMassParameters: async (
    params: Record<string, number>
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

  // ==================== Car Number Parameters ====================
  getCarNumberParameters: async (): Promise<{
    carNumberParameters: TrainNumberParams;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/carnumber`);
    if (!res.ok)
      throw new Error(`Failed to get car number parameters: ${res.status}`);
    return res.json();
  },

  updateCarNumberParameters: async (
    params: TrainNumberParams
  ): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/carnumber`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(params),
    });
    if (!res.ok)
      throw new Error(`Failed to update car number parameters: ${res.status}`);
    return res.json();
  },

  // ==================== Passenger Parameters ====================
  getPassengerParameters: async (): Promise<{
    passengerParameters: TrainPassangerParams;
    status: string;
  }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/passenger`);
    if (!res.ok)
      throw new Error(`Failed to get passenger parameters: ${res.status}`);
    return res.json();
  },

  updatePassengerParameters: async (
    params: TrainPassangerParams
  ): Promise<{ status: string; message: string }> => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/passenger`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(params),
    });
    if (!res.ok)
      throw new Error(`Failed to update passenger parameters: ${res.status}`);
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

  getSimulationResults: async (): Promise<SimulationResults> => {
    const res = await fetch(`${API_BASE_URL}/api/simulation/results`);
    if (!res.ok)
      throw new Error(`Failed to get simulation results: ${res.status}`);
    return res.json();
  },
};

// Re-export types for convenience
export type {
  TrainConstantParams,
  TrainMassParams,
  TrainPassangerParams,
  TrainNumberParams,
  TrackParams,
  RunningParams,
  ElectricalParams,
};

export type {
  SimulationConfig,
  SimulationResults,
} from "@/types/simulation-params";
