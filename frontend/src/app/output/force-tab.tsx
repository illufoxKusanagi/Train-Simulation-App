"use client";

import { useMemo } from "react";
import type { ChartConfig } from "@/components/ui/chart";
import type { SimulationResults } from "@/services/api";
import ChartTab, { type DownloadFn } from "./chart-tab";

const chartConfig = {
  motorForce: {
    label: "Motor Force (N)",
    color: "var(--chart-1)",
  },
  totalResistance: {
    label: "Total Force (N)",
    color: "var(--chart-2)",
  },
  motorResistance: {
    label: "F Resistance (N)",
    color: "var(--chart-3)",
  },
  motorResistancesOption1: {
    label: "Run Res Option 1 (N)",
    color: "var(--chart-4)",
  },
  motorResistancesOption2: {
    label: "Run Res Option 2 (N)",
    color: "var(--chart-5)",
  },
  motorResistancesOption3: {
    label: "Run Res Option 3 (N)",
    color: "#10b981",
  },
  motorResistancesOption4: {
    label: "Run Res Option 4 (N)",
    color: "#f43f5e",
  },
} satisfies ChartConfig;

const BASE_EXPORT_COLUMNS: [string, string][] = [
  ["phase", "Phase"],
  ["iteration", "Iteration"],
  ["time", "Time (s)"],
  ["timeTotal", "Total time (s)"],
  ["speeds", "Speed (km/h)"],
  ["motorForce", "F Motor"],
  ["motorResistance", "F Res"],
  ["totalResistance", "F Total"],
  ["tractionForcePerMotor", "F Motor /TM"],
  ["resistancePerMotor", "F Res / TM"],
];

const STATIC_EXTRA_COLUMNS: [string, string][] = [
  ["motorResistancesOption1", "Run res at 0"],
  ["motorResistancesOption2", "Run res at 5"],
  ["motorResistancesOption3", "Run res at 10"],
  ["motorResistancesOption4", "Run res at 25"],
];

const BASE_LINES = [
  { dataKey: "motorForce", stroke: "var(--color-motorForce)" },
  { dataKey: "motorResistance", stroke: "var(--color-motorResistance)" },
  { dataKey: "totalResistance", stroke: "var(--color-totalResistance)" },
];

const STATIC_EXTRA_LINES = [
  {
    dataKey: "motorResistancesOption1",
    stroke: "var(--color-motorResistancesOption1)",
    strokeWidth: 1.5,
    strokeDasharray: "4 2",
  },
  {
    dataKey: "motorResistancesOption2",
    stroke: "var(--color-motorResistancesOption2)",
    strokeWidth: 1.5,
    strokeDasharray: "4 2",
  },
  {
    dataKey: "motorResistancesOption3",
    stroke: "var(--color-motorResistancesOption3)",
    strokeWidth: 1.5,
    strokeDasharray: "4 2",
  },
  {
    dataKey: "motorResistancesOption4",
    stroke: "var(--color-motorResistancesOption4)",
    strokeWidth: 1.5,
    strokeDasharray: "4 2",
  },
];

interface ForceTabProps {
  results: SimulationResults;
  onDownloadCSV: DownloadFn;
  onDownloadExcel: DownloadFn;
}

export default function ForceTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: ForceTabProps) {
  const simulationType = results.debugInfo?.simulationType || "dynamic";
  const isStatic = simulationType === "static";

  const exportColumns = useMemo(
    () =>
      isStatic
        ? [...BASE_EXPORT_COLUMNS, ...STATIC_EXTRA_COLUMNS]
        : BASE_EXPORT_COLUMNS,
    [isStatic],
  );

  const lines = useMemo(
    () => (isStatic ? [...BASE_LINES, ...STATIC_EXTRA_LINES] : BASE_LINES),
    [isStatic],
  );

  const exportKeys = useMemo(
    () => exportColumns.map(([key]) => key),
    [exportColumns],
  );

  return (
    <ChartTab
      results={results}
      chartKey="force"
      chartConfig={chartConfig}
      lines={lines}
      yAxisUnit="N"
      exportColumns={exportColumns}
      exportFilename="force_data"
      exportKeys={exportKeys}
      onDownloadCSV={onDownloadCSV}
      onDownloadExcel={onDownloadExcel}
    />
  );
}
