"use client";

import { useMemo } from "react";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import type { ChartConfig } from "@/components/ui/chart";
import type { SimulationResults } from "@/services/api";
import ChartTab, { type DownloadFn } from "./chart-tab";
import { useTranslations } from "next-intl";

const chartConfig = {
  distancesTotal: {
    label: "Distance (m)",
    color: "var(--chart-1)",
  },
} satisfies ChartConfig;

const EXPORT_COLUMNS: [string, string][] = [
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

const EXPORT_KEYS = EXPORT_COLUMNS.map(([key]) => key);

interface DistanceTabProps {
  results: SimulationResults;
  onDownloadCSV: DownloadFn;
  onDownloadExcel: DownloadFn;
}

export default function DistanceTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: DistanceTabProps) {
  const t = useTranslations("Outputs");
  const simulationType = results.debugInfo?.simulationType || "dynamic";
  const isStatic = simulationType === "static";

  const staticContent = useMemo(() => {
    if (!isStatic) return null;
    return (
      <div className="space-y-4">
        {/* Static simulation: distance summary cards */}
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>
                {t("charts.distance.distancePowering")}
              </CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.distanceTravelled ?? 0).toFixed(2)} m
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>
                {t("charts.distance.distanceBraking")}
              </CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.distanceOnBraking ?? 0).toFixed(2)} m
              </CardTitle>
            </CardHeader>
          </Card>
          <Card>
            <CardHeader className="pb-2">
              <CardDescription>
                {t("charts.distance.distanceEmergency")}
              </CardDescription>
              <CardTitle className="text-2xl">
                {(results.summary?.distanceOnEmergencyBraking ?? 0).toFixed(2)}{" "}
                m
              </CardTitle>
            </CardHeader>
          </Card>
        </div>

        {/* Static simulation: braking distance table */}
        {results.trackDistanceTable && (
          <Card>
            <CardHeader>
              <CardTitle>{t("charts.distance.trackAnalysis")}</CardTitle>
              <CardDescription>
                {t("charts.distance.trackAnalysisDescription")}
              </CardDescription>
            </CardHeader>
            <CardContent>
              <div className="overflow-x-auto">
                <table className="w-full border-collapse">
                  <thead>
                    <tr className="bg-secondary">
                      <th className="border border-border p-3 text-left"></th>
                      <th className="border border-border p-3 text-center">
                        {t("charts.distance.trackDistance")}
                      </th>
                      <th className="border border-border p-3 text-center">
                        {t("charts.distance.trackDistanceEB")}
                      </th>
                    </tr>
                  </thead>
                  <tbody>
                    {results.trackDistanceTable?.labels?.map((label, index) => (
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
  }, [isStatic, results, t]);

  if (isStatic) {
    return <div className="space-y-4">{staticContent}</div>;
  }

  return (
    <ChartTab
      results={results}
      chartKey="distance"
      chartConfig={chartConfig}
      lines={[
        { dataKey: "distancesTotal", stroke: "var(--color-distancesTotal)" },
      ]}
      yAxisUnit="m"
      exportColumns={EXPORT_COLUMNS}
      exportFilename="distance_data"
      exportKeys={EXPORT_KEYS}
      onDownloadCSV={onDownloadCSV}
      onDownloadExcel={onDownloadExcel}
    />
  );
}
