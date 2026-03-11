"use client";

import { useEffect, useState } from "react";
import PageLayout from "@/components/page-layout";
import { Button } from "@/components/ui/button";
import { Tabs, TabsList, TabsTrigger, TabsContent } from "@/components/ui/tabs";
import {
  Card,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { Download, AlertCircle } from "lucide-react";
import { Spinner } from "@/components/ui/spinner";
import { Skeleton } from "@/components/ui/skeleton";
import { Alert, AlertDescription } from "@/components/ui/alert";
import { api } from "@/services/api";
import type { SimulationResults } from "@/services/api";
import * as XLSX from "xlsx";
import { initializeQtWebChannel } from "@/lib/qt-webchannel";
import SpeedTab from "./speed-tab";
import PowerTab from "./power-tab";
import CurrentTab from "./current-tab";
import ForceTab from "./force-tab";
import DistanceTab from "./distance-tab";
import { toast } from "sonner";
import PowerPerMotorTab from "./power-per-motor-tab";

export default function OutputPage() {
  const [results, setResults] = useState<SimulationResults | null>(null);
  const [activeTab, setActiveTab] = useState<string>("speed");
  const [isLoading, setIsLoading] = useState(true);
  const [isExportingCSV, setIsExportingCSV] = useState(false);
  const [isExportingExcel, setIsExportingExcel] = useState(false);

  useEffect(() => {
    // Function to load results directly from the backend
    const loadResults = async () => {
      try {
        const statusResponse = await api.getSimulationStatus();
        if (statusResponse.summary) {
          const resultsResponse = await api.getSimulationResults();
          setResults({ ...resultsResponse, summary: statusResponse.summary });
        }
      } catch (error) {
        console.error("Failed to load simulation results:", error);
      } finally {
        setIsLoading(false);
      }
    };

    // Initial load
    loadResults();

    // Listen for simulation updates
    window.addEventListener("simulationUpdated", loadResults);

    // Cleanup
    return () => {
      window.removeEventListener("simulationUpdated", loadResults);
    };
  }, []);

  useEffect(() => {
    // Restore last active tab from localStorage
    const savedTab = localStorage.getItem("outputPageActiveTab");
    if (savedTab) {
      setActiveTab(savedTab);
    }

    // Initialize Qt WebChannel for native file dialogs
    initializeQtWebChannel()
      .then(() => {
        console.log("✅ Qt WebChannel initialized successfully");
      })
      .catch((error) => {
        console.warn("⚠️ Qt WebChannel initialization failed:", error);
      });
  }, []);

  // Exact column order matching csv_output_handler.cpp header
  const CSV_COLUMNS: [string, string][] = [
    ["phase", "Phase"],
    ["iteration", "Iteration"],
    ["time", "Time (s)"],
    ["timeTotal", "Total time (s)"],
    ["distances", "Distance (m)"],
    ["distancesTotal", "TotalDistance (m)"],
    ["odos", "Odo (m)"],
    ["brakingDistances", "Braking Distance"],
    ["slopes", "Slope"],
    ["radiuses", "Radius"],
    ["speeds", "Speed (km/h)"],
    ["speedLimits", "Speed Limit(km/h)"],
    ["speedsSi", "Speed (m/s)"],
    ["accelerations", "Acceleration (km/h/s)"],
    ["accelerationsSi", "Acceleration (m/s2)"],
    ["motorForce", "F Motor"],
    ["motorResistance", "F Res"],
    ["totalResistance", "F Total"],
    ["tractionForcePerMotor", "F Motor /TM"],
    ["resistancePerMotor", "F Res / TM"],
    ["torque", "Torque"],
    ["rpm", "RPM"],
    ["powerWheel", "P Wheel"],
    ["powerMotorOut", "P_motor Out"],
    ["powerMotorIn", "P_motor In"],
    ["vvvfPowers", "P_vvvf"],
    ["catenaryPowers", "P_catenary"],
    ["catenaryCurrents", "Catenary current"],
    ["vvvfCurrents", "VVVF current"],
    ["energyConsumptions", "Energy Consumption"],
    ["energyPowerings", "Energy of Powering"],
    ["powerMotorOutputPerMotor", "P_motor Out per motor"],
    ["energyAps", "Energy of APS"],
    ["energyCatenaries", "Energy Catenary"],
    ["motorResistancesOption1", "Run res at 0"],
    ["motorResistancesOption2", "Run res at 5"],
    ["motorResistancesOption3", "Run res at 10"],
    ["motorResistancesOption4", "Run res at 25"],
  ];

  const downloadCSV = async (
    data: unknown[],
    filename: string,
    columns: [string, string][],
  ) => {
    console.log("🚀 === QT WEBENGINE CSV DOWNLOAD ===");
    console.log("Data points:", data?.length);

    if (!data || !Array.isArray(data) || data.length === 0) {
      toast("❌ No valid simulation data available for CSV download");
      return;
    }

    try {
      const firstItem = data[0] as Record<string, unknown>;
      if (!firstItem || typeof firstItem !== "object") {
        throw new Error("Invalid simulation data structure");
      }

      const csvHeaders = columns.map(([, header]) => header);
      const csvRows = [csvHeaders.join(",")];

      for (const rawItem of data) {
        const item = rawItem as Record<string, unknown>;
        const csvRow = columns.map(([key]) => {
          const value = item[key] ?? "";
          if (typeof value === "string" && value.includes(",")) {
            return `"${value.replace(/"/g, '""')}"`;
          }
          return String(value);
        });
        csvRows.push(csvRow.join(","));
      }

      const csvContent = csvRows.join("\n");
      console.log(
        "📊 Generated CSV (",
        csvContent.length,
        "chars, ",
        csvRows.length - 1,
        "data rows)",
      );

      // Use native file dialog if available
      if (typeof window !== "undefined" && window.fileBridge) {
        try {
          const result = await window.fileBridge.saveFileDialog(
            csvContent,
            filename,
            "CSV Files (*.csv);;All Files (*.*)",
          );

          if (result.success) {
            toast(`✅ File saved: ${result.filepath}`);
            return;
          } else if (result.error !== "User cancelled file dialog") {
            toast(`❌ Save failed: ${result.error}`);
          }
          return;
        } catch (error) {
          console.error("File save error:", error);
        }
      }

      // File System Access API not available
      console.log("❌ No native file dialog available");

      // Fallback: Try to trigger download and show instruction
      console.log("💾 Using fallback blob download");
      const blob = new Blob([csvContent], { type: "text/csv;charset=utf-8;" });
      const url = URL.createObjectURL(blob);

      const link = document.createElement("a");
      link.href = url;
      link.download = filename;
      link.style.display = "none";

      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);
      URL.revokeObjectURL(url);

      // For now, file goes to Downloads. To enable file picker:
      toast(
        `✅ CSV file downloaded successfully!\n📁 Location: ~/Downloads/${filename}\n\n🔧 To choose save location, you need Qt WebChannel integration.\nSee: NATIVE-FILE-DIALOG-INTEGRATION.md`,
      );
    } catch (error) {
      console.error("💥 CSV Download Failed:", error);
      toast(
        `❌ CSV Download Error: ${
          error instanceof Error ? error.message : String(error)
        }`,
      );
    }
  };

  const downloadExcel = async (
    data: unknown[],
    filename: string,
    columns: [string, string][],
  ) => {
    console.log("📊 === QT WEBENGINE EXCEL DOWNLOAD ===");
    console.log("Data points:", data?.length);

    if (!data || !Array.isArray(data) || data.length === 0) {
      toast("❌ No valid simulation data available for Excel download");
      return;
    }

    try {
      if (typeof XLSX === "undefined") {
        throw new Error("XLSX library not loaded");
      }

      const excelData = data.map((rawItem: unknown) => {
        const item = rawItem as Record<string, unknown>;
        return Object.fromEntries(
          columns.map(([key, header]) => [header, item[key] ?? ""]),
        );
      });

      console.log("📈 Creating Excel workbook with", excelData.length, "rows");

      // Create worksheet and workbook
      const worksheet = XLSX.utils.json_to_sheet(excelData);
      const workbook = XLSX.utils.book_new();
      XLSX.utils.book_append_sheet(
        workbook,
        worksheet,
        "Train Simulation Data",
      );

      // QT WebEngine Desktop Excel Save Integration
      console.log("🖥️ Desktop App - Attempting Qt Excel Dialog Integration");

      // Check if Qt WebChannel bridge is available
      if (typeof window !== "undefined" && window.fileBridge) {
        console.log(
          "🔌 Qt WebChannel fileBridge detected - using native Excel file dialog",
        );
        try {
          const buffer = XLSX.write(workbook, {
            bookType: "xlsx",
            type: "array",
          });
          const uint8Array = new Uint8Array(buffer);
          const dataArray = Array.from(uint8Array); // Convert to regular array for Qt WebChannel

          const result = await window.fileBridge.saveBinaryFileDialog(
            dataArray,
            filename,
            "Excel Files (*.xlsx);;All Files (*.*)",
          );

          if (result && result.success) {
            toast(
              `✅ Excel file saved successfully!\n📁 Location: ${result.filepath}`,
            );
            return;
          } else if (result && result.error) {
            console.warn("Qt Excel save failed:", result.error);
            if (result.error === "User cancelled file dialog") {
              return; // User cancelled - don't show error
            }
            toast(`❌ Failed to save Excel file: ${result.error}`);
            return;
          }
        } catch (qtError) {
          console.log("⚠️ Qt WebChannel Excel save error:", qtError);
          toast(
            `❌ Qt WebChannel error: ${
              qtError instanceof Error ? qtError.message : String(qtError)
            }`,
          );
          return;
        }
      } else {
        console.log("⚠️ Qt WebChannel fileBridge not available");
        toast(
          "❌ Native file dialog not available. Qt WebChannel integration required.",
        );
        return;
      }

      // File System Access API not available for Excel
      console.log("❌ No native Excel dialog available");

      // Fallback: Standard XLSX download
      console.log("💾 Using XLSX.writeFile fallback method");
      XLSX.writeFile(workbook, filename);

      // For now, file goes to Downloads. To enable file picker:
      toast(
        `✅ Excel file downloaded successfully!\n📁 Location: ~/Downloads/${filename}\n\n🔧 To choose save location, you need Qt WebChannel integration.\nSee: NATIVE-FILE-DIALOG-INTEGRATION.md`,
      );
    } catch (error) {
      console.error("💥 Excel Download Failed:", error);
      toast(
        `❌ Excel Download Error: ${
          error instanceof Error ? error.message : String(error)
        }`,
      );
    }
  };

  // Validate results and prevent empty array errors
  const hasValidResults =
    results &&
    results.results &&
    Array.isArray(results.results) &&
    results.results.length > 0;

  if (isLoading) {
    return (
      <PageLayout>
        <div className="flex flex-col gap-6 h-full w-full p-6">
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
            {Array.from({ length: 4 }).map((_, i) => (
              <Skeleton key={i} className="h-24 w-full rounded-xl" />
            ))}
          </div>
          <Skeleton className="h-10 w-72 rounded-lg" />
          <Skeleton className="h-[360px] w-full rounded-xl" />
        </div>
      </PageLayout>
    );
  }

  if (!hasValidResults) {
    return (
      <PageLayout>
        <div className="flex flex-col items-center justify-center h-[60vh] gap-4">
          <Alert className="max-w-md">
            <AlertCircle className="h-4 w-4" />
            <AlertDescription>
              No valid simulation results available. Run a simulation first
              using the buttons in the topbar or generate test data below.
            </AlertDescription>
          </Alert>
          <Button
            onClick={() => {
              // Generate mock data for testing
              const mockData: SimulationResults = {
                status: "success",
                totalPoints: 10,
                returnedPoints: 10,
                results: Array.from({ length: 10 }, (_, i) => ({
                  phase: `Phase ${i + 1}`,
                  iteration: i + 1,
                  time: i * 0.1,
                  timeTotal: i * 0.1,
                  distances: i * 10, // API field name
                  distancesTotal: i * 10, // API field name
                  odos: i * 10,
                  brakingDistances: i * 5,
                  slopes: 0,
                  radiuses: 1000,
                  speeds: 60 + i * 5, // API field name
                  speedLimits: 120,
                  speedsSi: (60 + i * 5) / 3.6, // API field name
                  accelerations: 0.5,
                  accelerationsSi: 0.5,
                  motorForce: 1000 + i * 50,
                  motorResistance: 100,
                  totalResistance: 150,
                  tractionForcePerMotor: 250,
                  resistancePerMotor: 37.5,
                  torque: 500,
                  rpm: 1000 + i * 100,
                  powerWheel: 500,
                  powerMotorOut: 520,
                  powerMotorIn: 600,
                  vvvfPowers: 600 + i * 20,
                  vvvfCurrents: 100 + i * 5,
                  catenaryPowers: 620 + i * 20,
                  catenaryCurrents: 102 + i * 5,
                  energyConsumptions: 50 + i * 2,
                  energyPowerings: 45 + i * 2,
                  // energyRegenerations: 0,
                  energyAps: 5,
                  energyCatenaries: 50 + i * 2,
                  motorResistancesOption1: 0,
                  motorResistancesOption2: 5,
                  motorResistancesOption3: 15,
                  motorResistancesOption4: 25,
                  powerMotorOutputPerMotor: 100 * i,
                })),
                summary: {
                  maxSpeed: 105,
                  distanceTravelled: 90,
                  maxTractionEffort: 1450,
                  adhesion: 0.179,
                  maxCatenaryPower: 800,
                  maxVvvfPower: 780,
                  maxCatenaryCurrent: 145,
                  maxVvvfCurrent: 142,
                  maxCurrentTime: 0.9,
                  maxPowerTime: 0.8,
                  maxEnergyConsumption: 68,
                  maxEnergyPowering: 63,
                  maxMotorPowerPerMotor: 320,
                  maxEnergyAps: 5,
                },
              };

              setResults(mockData);
              console.log("Mock data generated:", mockData);
            }}
            variant="outline"
            className="mt-4"
          >
            Generate Test Data
          </Button>
        </div>
      </PageLayout>
    );
  }

  return (
    <PageLayout>
      <div className="flex flex-col gap-6 h-full w-full p-6 overflow-y-auto custom-scrollbar">
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
              <CardDescription>Max Power per Motor</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxMotorPowerPerMotor ?? 0).toFixed(2)} kW
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
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Time at Peak Power</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.maxPowerTime ?? 0).toFixed(2)} s
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Travel Time</CardDescription>
              <CardTitle className="text-2xl">
                {results.results.length > 0
                  ? results.results[
                      results.results.length - 1
                    ].timeTotal.toFixed(1)
                  : "0.0"}{" "}
                s
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
          <TabsList className="flex w-full gap-1">
            <TabsTrigger value="speed">Speed</TabsTrigger>
            <TabsTrigger value="power">Power</TabsTrigger>
            <TabsTrigger value="power-per-motor">
              Power Per Motor Train
            </TabsTrigger>
            <TabsTrigger value="current">Current</TabsTrigger>
            <TabsTrigger value="force">Force</TabsTrigger>
            <TabsTrigger value="distance">Distance</TabsTrigger>
          </TabsList>

          <TabsContent value="speed">
            <SpeedTab
              results={results}
              onDownloadCSV={downloadCSV}
              onDownloadExcel={downloadExcel}
            />
          </TabsContent>

          <TabsContent value="power">
            <PowerTab
              results={results}
              onDownloadCSV={downloadCSV}
              onDownloadExcel={downloadExcel}
            />
          </TabsContent>

          <TabsContent value="power-per-motor">
            <PowerPerMotorTab
              results={results}
              onDownloadCSV={downloadCSV}
              onDownloadExcel={downloadExcel}
            />
          </TabsContent>

          <TabsContent value="current">
            <CurrentTab
              results={results}
              onDownloadCSV={downloadCSV}
              onDownloadExcel={downloadExcel}
            />
          </TabsContent>

          <TabsContent value="force">
            <ForceTab
              results={results}
              onDownloadCSV={downloadCSV}
              onDownloadExcel={downloadExcel}
            />
          </TabsContent>

          <TabsContent value="distance">
            <DistanceTab
              results={results}
              onDownloadCSV={downloadCSV}
              onDownloadExcel={downloadExcel}
            />
          </TabsContent>
        </Tabs>

        {/* Download All Buttons - QT WEBENGINE COMPATIBLE */}
        <div className="flex justify-end gap-2 flex-wrap">
          <Button
            variant="outline"
            disabled={isExportingCSV}
            onClick={async () => {
              setIsExportingCSV(true);
              await downloadCSV(
                results.results,
                "train_simulation_all_data.csv",
                CSV_COLUMNS,
              );
              setIsExportingCSV(false);
            }}
          >
            {isExportingCSV ? (
              <Spinner className="h-4 w-4 mr-2" />
            ) : (
              <Download className="h-4 w-4 mr-2" />
            )}
            {isExportingCSV ? "Saving..." : "Download All Data (CSV)"}
          </Button>

          <Button
            disabled={isExportingExcel}
            onClick={async () => {
              setIsExportingExcel(true);
              await downloadExcel(
                results.results,
                "train_simulation_all_data.xlsx",
                CSV_COLUMNS,
              );
              setIsExportingExcel(false);
            }}
          >
            {isExportingExcel ? (
              <Spinner className="h-4 w-4 mr-2" />
            ) : (
              <Download className="h-4 w-4 mr-2" />
            )}
            {isExportingExcel ? "Saving..." : "Download All Data (Excel)"}
          </Button>
        </div>
      </div>
    </PageLayout>
  );
}
