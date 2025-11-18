"use client";

import { useEffect, useState } from "react";
import PageLayout from "@/components/page-layout";
import { Button } from "@/components/ui/button";
import { Tabs, TabsList, TabsTrigger, TabsContent } from "@/components/ui/tabs";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from "recharts";
import { Download, AlertCircle } from "lucide-react";
import { Alert, AlertDescription } from "@/components/ui/alert";
import type { SimulationResults } from "@/services/api";

export default function OutputPage() {
  const [results, setResults] = useState<SimulationResults | null>(null);
  const [activeTab, setActiveTab] = useState<string>("speed");

  useEffect(() => {
    // Load simulation results from sessionStorage
    const stored = sessionStorage.getItem("simulationResults");
    if (stored) {
      try {
        setResults(JSON.parse(stored));
      } catch (error) {
        console.error("Failed to parse simulation results:", error);
      }
    }

    // Restore last active tab from localStorage
    const savedTab = localStorage.getItem("outputPageActiveTab");
    if (savedTab) {
      setActiveTab(savedTab);
    }
  }, []);

  const downloadCSV = (data: unknown[], filename: string) => {
    if (!data || data.length === 0) return;

    const headers = Object.keys(data[0] as Record<string, unknown>);
    const csv = [
      headers.join(","),
      ...data.map((row) =>
        headers
          .map((header) => (row as Record<string, unknown>)[header])
          .join(",")
      ),
    ].join("\n");

    const blob = new Blob([csv], { type: "text/csv" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = filename;
    a.click();
    URL.revokeObjectURL(url);
  };

  if (!results) {
    return (
      <PageLayout>
        <div className="flex flex-col items-center justify-center h-[60vh] gap-4">
          <Alert className="max-w-md">
            <AlertCircle className="h-4 w-4" />
            <AlertDescription>
              No simulation results available. Run a simulation first using the
              buttons in the topbar.
            </AlertDescription>
          </Alert>
        </div>
      </PageLayout>
    );
  }

  return (
    <PageLayout>
      <div className="flex flex-col gap-6 w-full">
        {/* Summary Cards - All 8 fields from backend */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Max Speed</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxSpeed ?? 0).toFixed(2)} km/h
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Distance Travelled</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.distanceTravelled ?? 0).toFixed(2)} m
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Max VVVF Power</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxVvvfPower ?? 0).toFixed(2)} kW
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Max Catenary Power</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxCatenaryPower ?? 0).toFixed(2)} kW
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Max Traction Effort</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxTractionEffort ?? 0).toFixed(2)} kN
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Max Energy Consumption</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxEnergyConsumption ?? 0).toFixed(2)} kWh
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Max Energy Regen</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxEnergyRegen ?? 0).toFixed(2)} kWh
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Max VVVF Current</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxVvvfCurrent ?? 0).toFixed(2)} A
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Max Catenary Current</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxCatenaryCurrent ?? 0).toFixed(2)} A
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Adhesion Coefficient</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.adhesion ?? 0).toFixed(3)}
              </CardTitle>
            </CardHeader>
          </Card>
        </div>

        {/* Charts */}
        <Tabs
          value={activeTab}
          onValueChange={(value) => {
            setActiveTab(value);
            localStorage.setItem("outputPageActiveTab", value);
          }}
          className="w-full"
        >
          <TabsList className="grid w-full grid-cols-5 gap-1">
            <TabsTrigger value="speed">Speed</TabsTrigger>
            <TabsTrigger value="power">Power</TabsTrigger>
            <TabsTrigger value="current">Current</TabsTrigger>
            <TabsTrigger value="force">Force</TabsTrigger>
            <TabsTrigger value="distance">Distance</TabsTrigger>
          </TabsList>

          <TabsContent value="speed" className="space-y-4">
            <Card>
              <CardHeader>
                <CardTitle>Train Speed vs Time</CardTitle>
                <CardDescription>
                  Speed profile throughout the simulation
                </CardDescription>
              </CardHeader>
              <CardContent>
                <ResponsiveContainer width="100%" height={400}>
                  <LineChart data={results.results}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis
                      dataKey="time"
                      label={{
                        value: "Time (s)",
                        position: "insideBottom",
                        offset: -5,
                      }}
                    />
                    <YAxis
                      label={{
                        value: "Speed (km/h)",
                        angle: -90,
                        position: "insideLeft",
                      }}
                    />
                    <Tooltip />
                    <Legend />
                    <Line
                      type="monotone"
                      dataKey="speed"
                      stroke="#8884d8"
                      name="Speed (km/h)"
                      dot={false}
                    />
                  </LineChart>
                </ResponsiveContainer>
                <div className="flex justify-end mt-4">
                  <Button
                    size="sm"
                    onClick={() =>
                      downloadCSV(results.results, "speed_data.csv")
                    }
                  >
                    <Download className="h-4 w-4 mr-2" />
                    Download Data
                  </Button>
                </div>
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="power" className="space-y-4">
            <Card>
              <CardHeader>
                <CardTitle>Power Consumption</CardTitle>
                <CardDescription>
                  VVVF and Catenary power over time
                </CardDescription>
              </CardHeader>
              <CardContent>
                <ResponsiveContainer width="100%" height={400}>
                  <LineChart data={results.results}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis
                      dataKey="time"
                      label={{
                        value: "Time (s)",
                        position: "insideBottom",
                        offset: -5,
                      }}
                    />
                    <YAxis
                      label={{
                        value: "Power (kW)",
                        angle: -90,
                        position: "insideLeft",
                      }}
                    />
                    <Tooltip />
                    <Legend />
                    <Line
                      type="monotone"
                      dataKey="vvvfPower"
                      stroke="#82ca9d"
                      name="VVVF Power (kW)"
                      dot={false}
                    />
                    <Line
                      type="monotone"
                      dataKey="catenaryPower"
                      stroke="#ffc658"
                      name="Catenary Power (kW)"
                      dot={false}
                    />
                  </LineChart>
                </ResponsiveContainer>
                <div className="flex justify-end mt-4">
                  <Button
                    size="sm"
                    onClick={() =>
                      downloadCSV(results.results, "power_data.csv")
                    }
                  >
                    <Download className="h-4 w-4 mr-2" />
                    Download Data
                  </Button>
                </div>
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="current" className="space-y-4">
            <Card>
              <CardHeader>
                <CardTitle>Current Draw</CardTitle>
                <CardDescription>
                  VVVF and Catenary current over time
                </CardDescription>
              </CardHeader>
              <CardContent>
                <ResponsiveContainer width="100%" height={400}>
                  <LineChart data={results.results}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis
                      dataKey="time"
                      label={{
                        value: "Time (s)",
                        position: "insideBottom",
                        offset: -5,
                      }}
                    />
                    <YAxis
                      label={{
                        value: "Current (A)",
                        angle: -90,
                        position: "insideLeft",
                      }}
                    />
                    <Tooltip />
                    <Legend />
                    <Line
                      type="monotone"
                      dataKey="vvvfCurrent"
                      stroke="#8884d8"
                      name="VVVF Current (A)"
                      dot={false}
                    />
                    <Line
                      type="monotone"
                      dataKey="catenaryCurrent"
                      stroke="#82ca9d"
                      name="Catenary Current (A)"
                      dot={false}
                    />
                  </LineChart>
                </ResponsiveContainer>
                <div className="flex justify-end mt-4">
                  <Button
                    size="sm"
                    onClick={() =>
                      downloadCSV(results.results, "current_data.csv")
                    }
                  >
                    <Download className="h-4 w-4 mr-2" />
                    Download Data
                  </Button>
                </div>
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="force" className="space-y-4">
            <Card>
              <CardHeader>
                <CardTitle>Forces</CardTitle>
                <CardDescription>
                  Tractive effort and resistance forces
                </CardDescription>
              </CardHeader>
              <CardContent>
                <ResponsiveContainer width="100%" height={400}>
                  <LineChart data={results.results}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis
                      dataKey="time"
                      label={{
                        value: "Time (s)",
                        position: "insideBottom",
                        offset: -5,
                      }}
                    />
                    <YAxis
                      label={{
                        value: "Force (kN)",
                        angle: -90,
                        position: "insideLeft",
                      }}
                    />
                    <Tooltip />
                    <Legend />
                    <Line
                      type="monotone"
                      dataKey="tractiveEffort"
                      stroke="#8884d8"
                      name="Tractive Effort (kN)"
                      dot={false}
                    />
                    <Line
                      type="monotone"
                      dataKey="totalResistance"
                      stroke="#ff7c7c"
                      name="Total Resistance (kN)"
                      dot={false}
                    />
                  </LineChart>
                </ResponsiveContainer>
                <div className="flex justify-end mt-4">
                  <Button
                    size="sm"
                    onClick={() =>
                      downloadCSV(results.results, "force_data.csv")
                    }
                  >
                    <Download className="h-4 w-4 mr-2" />
                    Download Data
                  </Button>
                </div>
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="distance" className="space-y-4">
            {/* Track Distance Table (Static Simulation) */}
            {results.trackDistanceTable && (
              <Card>
                <CardHeader>
                  <CardTitle>Track Distance Analysis</CardTitle>
                  <CardDescription>
                    Distance on powering and emergency braking scenarios
                  </CardDescription>
                </CardHeader>
                <CardContent>
                  <div className="overflow-x-auto">
                    <table className="w-full border-collapse">
                      <thead>
                        <tr className="bg-secondary">
                          <th className="border border-border p-3 text-left"></th>
                          <th className="border border-border p-3 text-center">
                            Track distance (m)
                          </th>
                          <th className="border border-border p-3 text-center">
                            Track distance on EB (m)
                          </th>
                        </tr>
                      </thead>
                      <tbody>
                        {results.trackDistanceTable.labels.map(
                          (label, index) => (
                            <tr key={label} className="hover:bg-accent">
                              <td className="border border-border p-3 font-medium">
                                {label}
                              </td>
                              <td className="border border-border p-3 text-center">
                                {typeof results.trackDistanceTable
                                  ?.normalBraking[index] === "number"
                                  ? results.trackDistanceTable.normalBraking[
                                      index
                                    ].toFixed(3)
                                  : "N/A"}
                              </td>
                              <td className="border border-border p-3 text-center">
                                {typeof results.trackDistanceTable
                                  ?.emergencyBraking[index] === "number"
                                  ? results.trackDistanceTable.emergencyBraking[
                                      index
                                    ].toFixed(3)
                                  : "N/A"}
                              </td>
                            </tr>
                          )
                        )}
                      </tbody>
                    </table>
                  </div>
                </CardContent>
              </Card>
            )}

            <Card>
              <CardHeader>
                <CardTitle>Distance & Acceleration</CardTitle>
                <CardDescription>
                  Position and acceleration profile
                </CardDescription>
              </CardHeader>
              <CardContent>
                <ResponsiveContainer width="100%" height={400}>
                  <LineChart data={results.results}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis
                      dataKey="time"
                      label={{
                        value: "Time (s)",
                        position: "insideBottom",
                        offset: -5,
                      }}
                    />
                    <YAxis
                      yAxisId="left"
                      label={{
                        value: "Distance (m)",
                        angle: -90,
                        position: "insideLeft",
                      }}
                    />
                    <YAxis
                      yAxisId="right"
                      orientation="right"
                      label={{
                        value: "Acceleration (m/s²)",
                        angle: 90,
                        position: "insideRight",
                      }}
                    />
                    <Tooltip />
                    <Legend />
                    <Line
                      yAxisId="left"
                      type="monotone"
                      dataKey="distance"
                      stroke="#8884d8"
                      name="Distance (m)"
                      dot={false}
                    />
                    <Line
                      yAxisId="right"
                      type="monotone"
                      dataKey="acceleration"
                      stroke="#82ca9d"
                      name="Acceleration (m/s²)"
                      dot={false}
                    />
                  </LineChart>
                </ResponsiveContainer>
                <div className="flex justify-end mt-4">
                  <Button
                    size="sm"
                    onClick={() =>
                      downloadCSV(results.results, "distance_data.csv")
                    }
                  >
                    <Download className="h-4 w-4 mr-2" />
                    Download Data
                  </Button>
                </div>
              </CardContent>
            </Card>
          </TabsContent>
        </Tabs>

        {/* Download All Button */}
        <div className="flex justify-end">
          <Button
            onClick={() =>
              downloadCSV(results.results, "all_simulation_data.csv")
            }
          >
            <Download className="h-4 w-4 mr-2" />
            Download All Data
          </Button>
        </div>
      </div>
    </PageLayout>
  );
}
