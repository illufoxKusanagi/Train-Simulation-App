import axios, { AxiosInstance } from "axios";

const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || "http://localhost:8080";

class TrainSimulationAPI {
  private client: AxiosInstance;

  constructor() {
    this.client = axios.create({
      baseURL: API_BASE_URL,
      timeout: 10000,
      headers: {
        "Content-Type": "application/json",
      },
    });
  }

  // Health check
  async checkHealth(): Promise<{ status: string; dataStatus: string }> {
    const response = await this.client.get("/api/health");
    return response.data;
  }

  // Add more API methods as needed
  async getTrainParameters(): Promise<any> {
    const response = await this.client.get("/api/parameters/train");
    return response.data;
  }

  async updateTrainParameters(params: any): Promise<any> {
    const response = await this.client.post("/api/parameters/train", params);
    return response.data;
  }
}

export const api = new TrainSimulationAPI();
