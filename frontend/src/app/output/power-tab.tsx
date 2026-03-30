"use client";

import type { ChartConfig } from "@/components/ui/chart";
import type { SimulationResults } from "@/services/api";
import ChartTab, { type DownloadFn } from "./chart-tab";

const chartConfig = {
  vvvfPowers: {
    label: "VVVF Power (kW)",
    color: "var(--chart-1)",
  },
  catenaryPowers: {
    label: "Catenary Power (kW)",
    color: "var(--chart-2)",
  },
} satisfies ChartConfig;

const EXPORT_COLUMNS: [string, string][] = [
  ["phase", "Phase"],
  ["iteration", "Iteration"],
  ["time", "Time (s)"],
  ["timeTotal", "Total time (s)"],
  ["speeds", "Speed (km/h)"],
  ["powerWheel", "P Wheel"],
  ["powerMotorOut", "P_motor Out"],
  ["powerMotorIn", "P_motor In"],
  ["vvvfPowers", "P_vvvf"],
  ["catenaryPowers", "P_catenary"],
];

const EXPORT_KEYS = EXPORT_COLUMNS.map(([key]) => key);

interface PowerTabProps {
  results: SimulationResults;
  onDownloadCSV: DownloadFn;
  onDownloadExcel: DownloadFn;
}

export default function PowerTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: PowerTabProps) {
  return (
    <ChartTab
      results={results}
      chartKey="power"
      chartConfig={chartConfig}
      lines={[
        { dataKey: "vvvfPowers", stroke: "var(--color-vvvfPowers)" },
        { dataKey: "catenaryPowers", stroke: "var(--color-catenaryPowers)" },
      ]}
      yAxisUnit="kW"
      exportColumns={EXPORT_COLUMNS}
      exportFilename="power_data"
      exportKeys={EXPORT_KEYS}
      onDownloadCSV={onDownloadCSV}
      onDownloadExcel={onDownloadExcel}
    />
  );
}
