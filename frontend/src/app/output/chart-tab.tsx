"use client";

import { useMemo, useRef } from "react";
import { Button } from "@/components/ui/button";
import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import {
  type ChartConfig,
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
import { useTranslations } from "next-intl";

// ─── Types ───────────────────────────────────────────────────────────
export interface LineDefinition {
  dataKey: string;
  stroke: string;
  strokeWidth?: number;
  strokeDasharray?: string;
}

export type DownloadFn = (
  data: unknown[],
  filename: string,
  columns: [string, string][],
) => void;

export interface ChartTabProps {
  /** Full simulation result object. */
  results: SimulationResults;
  /** i18n key path inside Outputs.charts, e.g. "speed" */
  chartKey: string;
  /** Recharts config object (legend colors / labels). */
  chartConfig: ChartConfig;
  /** Lines to render in the chart. */
  lines: LineDefinition[];
  /** Y-axis unit suffix, e.g. "km/h", "kW", "A", "N". Pass "" for no suffix. */
  yAxisUnit?: string;
  /** Columns for CSV / XLSX export. */
  exportColumns: [string, string][];
  /** Base filename (no extension) used for CSV, XLSX and chart image export. */
  exportFilename: string;
  /** Keys from each result item to include in the export data. */
  exportKeys: string[];
  /** Callback to download CSV. */
  onDownloadCSV: DownloadFn;
  /** Callback to download XLSX. */
  onDownloadExcel: DownloadFn;
  /** Optional: extra content rendered below the chart (before buttons). */
  children?: React.ReactNode;
}

// ─── Component ───────────────────────────────────────────────────────
export default function ChartTab({
  results,
  chartKey,
  chartConfig,
  lines,
  yAxisUnit = "",
  exportColumns,
  exportFilename,
  exportKeys,
  onDownloadCSV,
  onDownloadExcel,
  children,
}: ChartTabProps) {
  const t = useTranslations("Outputs");
  const data = useMemo(() => results.results || [], [results.results]);
  const simulationType = results.debugInfo?.simulationType || "dynamic";
  const chartRef = useRef<HTMLDivElement>(null);

  // ── Save chart as PNG (Qt bridge + browser fallback) ──────────────
  const saveImageHandler = async () => {
    if (chartRef.current === null) return;
    try {
      const dataUrl = await captureChartAsPng(chartRef.current);

      if (typeof window !== "undefined" && window.fileBridge) {
        const base64Data = dataUrl.split(",")[1];
        const binaryString = window.atob(base64Data);
        const len = binaryString.length;
        const bytes = new Uint8Array(len);
        for (let i = 0; i < len; i++) {
          bytes[i] = binaryString.charCodeAt(i);
        }

        const result = await window.fileBridge.saveBinaryFileDialog(
          Array.from(bytes),
          `${exportFilename}.png`,
          "Images (*.png);;All Files (*.*)",
        );

        if (result.success) {
          toast.success(`Saved to: ${result.filepath}`);
        } else if (result.error !== "User cancelled file dialog") {
          toast.error(`Failed: ${result.error}`);
        }
      } else {
        const link = document.createElement("a");
        link.download = `${exportFilename}.png`;
        link.href = dataUrl;
        link.click();
      }
    } catch (err) {
      toast.error("Failed to save chart image");
      console.error(err);
    }
  };

  // ── Tick generation ───────────────────────────────────────────────
  const tickInterval = simulationType === "static" ? 5 : 240;
  const maxTime =
    data.length > 0 ? (data[data.length - 1].timeTotal as number) : 0;

  const ticks = useMemo(() => {
    const arr = [];
    for (let i = 0; i <= maxTime; i += tickInterval) {
      arr.push(i);
    }
    return arr;
  }, [maxTime, tickInterval]);

  // ── Export data (pick only needed keys) ────────────────────────────
  const exportData = useMemo(
    () =>
      data.map((raw) => {
        const d = raw as unknown as Record<string, unknown>;
        return Object.fromEntries(exportKeys.map((key) => [key, d[key]]));
      }),
    [data, exportKeys],
  );

  // ── Y-axis formatter ─────────────────────────────────────────────
  const yAxisFormatter = yAxisUnit
    ? (value: number) => `${value} ${yAxisUnit}`
    : (value: number) => `${value}`;

  return (
    <div ref={chartRef} className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>{t(`charts.${chartKey}.title`)}</CardTitle>
          <CardDescription>
            {t(`charts.${chartKey}.description`)}
          </CardDescription>
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
                tickFormatter={yAxisFormatter}
              />
              <ChartTooltip
                cursor={false}
                content={<ChartTooltipContent />}
              />
              <ChartLegend content={<ChartLegendContent />} />
              {lines.map((line) => (
                <Line
                  key={line.dataKey}
                  type="monotone"
                  dataKey={line.dataKey}
                  stroke={line.stroke}
                  strokeWidth={line.strokeWidth ?? 2}
                  strokeDasharray={line.strokeDasharray}
                  dot={false}
                />
              ))}
            </LineChart>
          </ChartContainer>
        </CardContent>

        {children}

        <CardFooter>
          <div className="flex flex-row justify-end gap-2 mt-4 w-full flex-wrap">
            <Button size="sm" onClick={() => saveImageHandler()}>
              <Download className="h-4 w-4" />
              {t("downloadChartImage")}
            </Button>
            <Button
              size="sm"
              variant="outline"
              onClick={() =>
                onDownloadCSV(
                  exportData,
                  `${exportFilename}.csv`,
                  exportColumns,
                )
              }
            >
              <Download className="h-4 w-4 mr-2" />
              {t("downloadCsv")}
            </Button>
            <Button
              size="sm"
              onClick={() =>
                onDownloadExcel(
                  exportData,
                  `${exportFilename}.xlsx`,
                  exportColumns,
                )
              }
            >
              <Download className="h-4 w-4 mr-2" />
              {t("downloadExcel")}
            </Button>
          </div>
        </CardFooter>
      </Card>
    </div>
  );
}
