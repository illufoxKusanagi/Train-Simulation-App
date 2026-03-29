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
import { toast } from "sonner";
import { captureChartAsPng } from "@/lib/save-chart";
import { useTranslations } from "next-intl";

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

interface ForceTabProps {
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

export default function ForceTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: ForceTabProps) {
  const t = useTranslations("Outputs");
  const data = useMemo(() => results.results || [], [results.results]);
  const simulationType = results.debugInfo?.simulationType || "dynamic";
  const exportColumns = useMemo(
    () =>
      simulationType === "static"
        ? [...BASE_EXPORT_COLUMNS, ...STATIC_EXTRA_COLUMNS]
        : BASE_EXPORT_COLUMNS,
    [simulationType],
  );
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
          "force_chart.png",
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
        link.download = "force_chart.png";
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
          speeds: d.speeds,
          motorForce: d.motorForce,
          motorResistance: d.motorResistance,
          totalResistance: d.totalResistance,
          tractionForcePerMotor: d.tractionForcePerMotor,
          resistancePerMotor: d.resistancePerMotor,
          motorResistancesOption1: d.motorResistancesOption1,
          motorResistancesOption2: d.motorResistancesOption2,
          motorResistancesOption3: d.motorResistancesOption3,
          motorResistancesOption4: d.motorResistancesOption4,
        };
      }),
    [data],
  );

  return (
    <div ref={chartRef} className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>Forces vs Time</CardTitle>
          <CardDescription>Tractive and Resistive forces</CardDescription>
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
                tickFormatter={(value) => `${value} N`}
              />
              <ChartTooltip cursor={false} content={<ChartTooltipContent />} />
              <ChartLegend content={<ChartLegendContent />} />
              <Line
                type="monotone"
                dataKey="motorForce"
                stroke="var(--color-motorForce)"
                strokeWidth={2}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="motorResistance"
                stroke="var(--color-motorResistance)"
                strokeWidth={2}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="totalResistance"
                stroke="var(--color-totalResistance)"
                strokeWidth={2}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="motorResistancesOption1"
                stroke="var(--color-motorResistancesOption1)"
                strokeWidth={1.5}
                dot={false}
                strokeDasharray="4 2"
              />
              <Line
                type="monotone"
                dataKey="motorResistancesOption2"
                stroke="var(--color-motorResistancesOption2)"
                strokeWidth={1.5}
                dot={false}
                strokeDasharray="4 2"
              />
              <Line
                type="monotone"
                dataKey="motorResistancesOption3"
                stroke="var(--color-motorResistancesOption3)"
                strokeWidth={1.5}
                dot={false}
                strokeDasharray="4 2"
              />
              <Line
                type="monotone"
                dataKey="motorResistancesOption4"
                stroke="var(--color-motorResistancesOption4)"
                strokeWidth={1.5}
                dot={false}
                strokeDasharray="4 2"
              />
            </LineChart>
          </ChartContainer>
        </CardContent>
        <CardFooter>
          <div className="flex justify-end gap-2 mt-4 flex-wrap">
            <Button size="sm" onClick={() => saveImageHandler()}>
              <Download className="h-4 w-4" />
              {t("downloadChartImage")}
            </Button>
            <Button
              size="sm"
              variant="outline"
              onClick={() =>
                onDownloadCSV(exportData, "force_data.csv", exportColumns)
              }
            >
              <Download className="h-4 w-4 mr-2" />
              {t("downloadCsv")}
            </Button>
            <Button
              size="sm"
              onClick={() =>
                onDownloadExcel(exportData, "force_data.xlsx", exportColumns)
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
