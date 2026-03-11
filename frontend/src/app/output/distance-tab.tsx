"use client";

import { useMemo, useRef } from "react";
import { Button } from "@/components/ui/button";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import {
  ChartConfig,
  ChartContainer,
  ChartLegend,
  ChartLegendContent,
  ChartTooltip,
  ChartTooltipContent,
} from "@/components/ui/chart";
import { LineChart, Line, XAxis, YAxis, CartesianGrid } from "recharts";
import { Download } from "lucide-react";
import type { SimulationResults } from "@/services/api";
import { captureChartAsPng } from "@/lib/save-chart";
import { toast } from "sonner";

const chartConfig = {
  distancesTotal: {
    label: "Distance (m)",
    color: "var(--chart-1)",
  },
} satisfies ChartConfig;

const EXPORT_COLUMNS_DYNAMIC: [string, string][] = [
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
];

const EXPORT_COLUMNS_STATIC: [string, string][] = [...EXPORT_COLUMNS_DYNAMIC];

interface DistanceTabProps {
  results: SimulationResults;
  onDownloadCSV: (
    data: unknown[],
    filename: string,
    columns: [string, string][],
  ) => void;
  onDownloadExcel: (
    data: unknown[],
    filename: string,
    columns: [string, string][],
  ) => void;
}

export default function DistanceTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: DistanceTabProps) {
  const data = useMemo(() => results.results || [], [results.results]);
  const simulationType = results.debugInfo?.simulationType || "dynamic";
  const isStatic = simulationType === "static";
  const exportColumns = isStatic
    ? EXPORT_COLUMNS_STATIC
    : EXPORT_COLUMNS_DYNAMIC;
  const chartRef = useRef<HTMLDivElement>(null);

  const saveImageHandler = async () => {
    if (chartRef.current === null) return;

    try {
      const dataUrl = await captureChartAsPng(chartRef.current);

      // Check for native Qt bridge
      if (typeof window !== "undefined" && window.fileBridge) {
        // Remove header "data:image/png;base64,"
        const base64Data = dataUrl.split(",")[1];
        const binaryString = window.atob(base64Data);
        const len = binaryString.length;
        const bytes = new Uint8Array(len);
        for (let i = 0; i < len; i++) {
          bytes[i] = binaryString.charCodeAt(i);
        }

        const result = await window.fileBridge.saveBinaryFileDialog(
          Array.from(bytes),
          "distance_chart.png",
          "Images (*.png);;All Files (*.*)",
        );

        if (result.success) {
          toast.success(`Saved to: ${result.filepath}`);
        } else if (result.error !== "User cancelled file dialog") {
          toast.error(`Failed: ${result.error}`);
        }
      } else {
        // Fallback for web browser
        const link = document.createElement("a");
        link.download = "distance_chart.png";
        link.href = dataUrl;
        link.click();
      }
    } catch (err) {
      toast.error("Failed to save chart image");
      console.error(err);
    }
  };

  const tickInterval = simulationType === "static" ? 5 : 240;
  const maxTime =
    data.length > 0 ? (data[data.length - 1].timeTotal as number) : 0;

  const ticks = useMemo(() => {
    const t = [];
    for (let i = 0; i <= maxTime; i += tickInterval) {
      t.push(i);
    }
    return t;
  }, [maxTime, tickInterval]);

  const exportData = useMemo(
    () =>
      data.map((raw) => {
        const d = raw as unknown as Record<string, unknown>;
        return {
          phase: d.phase,
          iteration: d.iteration,
          time: d.time,
          timeTotal: d.timeTotal,
          distances: d.distances,
          distancesTotal: d.distancesTotal,
          odos: d.odos,
          brakingDistances: d.brakingDistances,
          slopes: d.slopes,
          radiuses: d.radiuses,
        };
      }),
    [data],
  );

  return (
    <div ref={chartRef} className="space-y-4">
      {!isStatic && (
        <Card>
          <CardHeader>
            <CardTitle>Distance vs Time</CardTitle>
            <CardDescription>Cumulative distance over time</CardDescription>
          </CardHeader>
          <CardContent>
            <ChartContainer
              config={chartConfig}
              className="aspect-auto h-[400px] w-full"
            >
              <LineChart data={data}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis
                  dataKey="timeTotal"
                  tickLine={false}
                  axisLine={false}
                  tickMargin={8}
                  tickFormatter={(value) => `${value}s`}
                  ticks={ticks}
                  type="number"
                  domain={["dataMin", "dataMax"]}
                />
                <YAxis
                  tickLine={false}
                  axisLine={false}
                  tickMargin={8}
                  tickFormatter={(value) => `${value} m`}
                />
                <ChartTooltip
                  cursor={false}
                  content={<ChartTooltipContent />}
                />
                <ChartLegend content={<ChartLegendContent />} />
                <Line
                  type="monotone"
                  dataKey="distancesTotal"
                  stroke="var(--color-distancesTotal)"
                  strokeWidth={2}
                  dot={false}
                />
              </LineChart>
            </ChartContainer>
            <div className="flex justify-end gap-2 mt-4">
              <Button size="sm" onClick={() => saveImageHandler()}>
                <Download className="h-4 w-4" />
                Download chart image
              </Button>
              <Button
                size="sm"
                variant="outline"
                onClick={() =>
                  onDownloadCSV(exportData, "distance_data.csv", exportColumns)
                }
              >
                <Download className="h-4 w-4 mr-2" />
                CSV
              </Button>
              <Button
                size="sm"
                onClick={() =>
                  onDownloadExcel(
                    exportData,
                    "distance_data.xlsx",
                    exportColumns,
                  )
                }
              >
                <Download className="h-4 w-4 mr-2" />
                Excel
              </Button>
            </div>
          </CardContent>
        </Card>
      )}

      {/* Static simulation: distance summary cards */}
      {isStatic && (
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Distance on Powering</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.distanceTravelled ?? 0).toFixed(2)} m
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Distance on Braking</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.distanceOnBraking ?? 0).toFixed(2)} m
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>Distance on Emergency Braking</CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.distanceOnEmergencyBraking ?? 0).toFixed(2)}{" "}
                m
              </CardTitle>
            </CardHeader>
          </Card>
        </div>
      )}

      {/* Static simulation: braking distance table */}
      {isStatic && results.trackDistanceTable && (
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
                  {results.trackDistanceTable.labels.map((label, index) => (
                    <tr key={label} className="hover:bg-accent">
                      <td className="border border-border p-3 font-medium">
                        {label}
                      </td>
                      <td className="border border-border p-3 text-center">
                        {typeof results.trackDistanceTable?.normalBraking[
                          index
                        ] === "number"
                          ? results.trackDistanceTable.normalBraking[
                              index
                            ].toFixed(3)
                          : "N/A"}
                      </td>
                      <td className="border border-border p-3 text-center">
                        {typeof results.trackDistanceTable?.emergencyBraking[
                          index
                        ] === "number"
                          ? results.trackDistanceTable.emergencyBraking[
                              index
                            ].toFixed(3)
                          : "N/A"}
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </CardContent>
        </Card>
      )}
    </div>
  );
}
