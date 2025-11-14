import { TrainParams } from "@/types/input-params";

const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || "http://localhost:8080";

export const api = {
  // Health check
  checkHealth: async () => {
    const res = await fetch(`${API_BASE_URL}/api/health`);
    return res.json();
  },

  // Get train parameters
  getTrainParameters: async () => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/train`);
    return res.json();
  },

  // Update train parameters
  updateTrainParameters: async (params: TrainParams) => {
    const res = await fetch(`${API_BASE_URL}/api/parameters/train`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ trainParameters: params }),
    });
    return res.json();
  },

  // Start simulation
  // startSimulation: async (params: SimulationParams) => {
  //   const res = await fetch(`${API_BASE_URL}/api/simulation/start`, {
  //     method: "POST",
  //     headers: { "Content-Type": "application/json" },
  //     body: JSON.stringify(params),
  //   });
  //   return res.json();
  // },

  // Get simulation results
  getSimulationResults: async () => {
    const res = await fetch(`${API_BASE_URL}/api/simulation/results`);
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
