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
  vvvfCurrents: {
    label: "VVVF Current (A)",
    color: "var(--chart-1)",
  },
  catenaryCurrents: {
    label: "Catenary Current (A)",
    color: "var(--chart-2)",
  },
} satisfies ChartConfig;

const EXPORT_COLUMNS: [string, string][] = [
  ["phase", "Phase"],
  ["iteration", "Iteration"],
  ["time", "Time (s)"],
  ["timeTotal", "Total time (s)"],
  ["speeds", "Speed (km/h)"],
  ["catenaryCurrents", "Catenary current"],
  ["vvvfCurrents", "VVVF current"],
];

interface CurrentTabProps {
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

export default function CurrentTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: CurrentTabProps) {
  const data = results.results || [];
  const simulationType = results.debugInfo?.simulationType || "dynamic";
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
          "current_chart.png",
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
        link.download = "current_chart.png";
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
          vvvfCurrents: d.vvvfCurrents,
          catenaryCurrents: d.catenaryCurrents,
        };
      }),
    [data],
  );

  // Validate data
  if (
    !results?.results ||
    results.results.length === 0 ||
    !results.results[0]
  ) {
    return (
      <Card>
        <CardContent className="pt-6">
          <div className="text-center text-muted-foreground p-8">
            No simulation data available to display.
          </div>
        </CardContent>
      </Card>
    );
  }

  return (
    <div ref={chartRef} className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>Current vs Time</CardTitle>
          <CardDescription>
            Current profiles throughout the simulation
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
                tickFormatter={(value) => `${value} A`}
              />
              <ChartTooltip cursor={false} content={<ChartTooltipContent />} />
              <ChartLegend content={<ChartLegendContent />} />
              <Line
                type="monotone"
                dataKey="vvvfCurrents"
                stroke="var(--color-vvvfCurrents)"
                strokeWidth={2}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="catenaryCurrents"
                stroke="var(--color-catenaryCurrents)"
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
                onDownloadCSV(exportData, "current_data.csv", EXPORT_COLUMNS)
              }
            >
              <Download className="h-4 w-4 mr-2" />
              CSV
            </Button>
            <Button
              size="sm"
              onClick={() =>
                onDownloadExcel(exportData, "current_data.xlsx", EXPORT_COLUMNS)
              }
            >
              <Download className="h-4 w-4 mr-2" />
              Excel
            </Button>
          </div>
        </CardContent>
      </Card>
    </div>
  );
}
