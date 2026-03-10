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

interface DistanceTabProps {
  results: SimulationResults;
  onDownloadCSV: (data: unknown[], filename: string) => void;
  onDownloadExcel: (data: unknown[], filename: string) => void;
}

export default function DistanceTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: DistanceTabProps) {
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

  return (
    <div ref={chartRef} className="space-y-4">
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
              <ChartTooltip cursor={false} content={<ChartTooltipContent />} />
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
                onDownloadCSV(results.results, "distance_data.csv")
              }
            >
              <Download className="h-4 w-4 mr-2" />
              CSV
            </Button>
            <Button
              size="sm"
              onClick={() =>
                onDownloadExcel(results.results, "distance_data.xlsx")
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
