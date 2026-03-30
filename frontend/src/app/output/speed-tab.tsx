"use client";

import type { ChartConfig } from "@/components/ui/chart";
import type { SimulationResults } from "@/services/api";
import ChartTab, { type DownloadFn } from "./chart-tab";

const chartConfig = {
  speeds: {
    label: "Speed (km/h)",
    color: "var(--chart-1)",
  },
} satisfies ChartConfig;

const EXPORT_COLUMNS: [string, string][] = [
  ["phase", "Phase"],
  ["iteration", "Iteration"],
  ["time", "Time (s)"],
  ["timeTotal", "Total time (s)"],
  ["speeds", "Speed (km/h)"],
  ["speedLimits", "Speed Limit(km/h)"],
  ["speedsSi", "Speed (m/s)"],
  ["accelerations", "Acceleration (km/h/s)"],
  ["accelerationsSi", "Acceleration (m/s2)"],
];

const EXPORT_KEYS = EXPORT_COLUMNS.map(([key]) => key);

interface SpeedTabProps {
  results: SimulationResults;
  onDownloadCSV: DownloadFn;
  onDownloadExcel: DownloadFn;
}

export default function SpeedTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: SpeedTabProps) {
  return (
    <ChartTab
      results={results}
      chartKey="speed"
      chartConfig={chartConfig}
      lines={[{ dataKey: "speeds", stroke: "#2563eb" }]}
      exportColumns={EXPORT_COLUMNS}
      exportFilename="speed_data"
      exportKeys={EXPORT_KEYS}
      onDownloadCSV={onDownloadCSV}
      onDownloadExcel={onDownloadExcel}
    />
  );
}
