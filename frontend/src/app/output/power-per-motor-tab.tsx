"use client";

import type { ChartConfig } from "@/components/ui/chart";
import type { SimulationResults } from "@/services/api";
import ChartTab, { type DownloadFn } from "./chart-tab";

const chartConfig = {
  powerMotorOutputPerMotor: {
    label: "Power per Motor (kW)",
    color: "var(--chart-3)",
  },
} satisfies ChartConfig;

const EXPORT_COLUMNS: [string, string][] = [
  ["phase", "Phase"],
  ["iteration", "Iteration"],
  ["time", "Time (s)"],
  ["timeTotal", "Total time (s)"],
  ["speeds", "Speed (km/h)"],
  ["powerMotorOutputPerMotor", "P_motor Out per motor"],
];

const EXPORT_KEYS = EXPORT_COLUMNS.map(([key]) => key);

interface PowerPerMotorTabProps {
  results: SimulationResults;
  onDownloadCSV: DownloadFn;
  onDownloadExcel: DownloadFn;
}

export default function PowerPerMotorTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: PowerPerMotorTabProps) {
  return (
    <ChartTab
      results={results}
      chartKey="powerPerMotor"
      chartConfig={chartConfig}
      lines={[
        {
          dataKey: "powerMotorOutputPerMotor",
          stroke: "var(--color-powerMotorOutputPerMotor)",
        },
      ]}
      yAxisUnit="kW"
      exportColumns={EXPORT_COLUMNS}
      exportFilename="power_per_motor_data"
      exportKeys={EXPORT_KEYS}
      onDownloadCSV={onDownloadCSV}
      onDownloadExcel={onDownloadExcel}
    />
  );
}
