"use client";

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

const chartConfig = {
  distanceTotal: {
    label: "Total Distance (m)",
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
  return (
    <div className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>Position</CardTitle>
          <CardDescription>Distance traveled over time</CardDescription>
        </CardHeader>
        <CardContent>
          <ChartContainer
            config={chartConfig}
            className="aspect-auto h-[400px] w-full"
          >
            <LineChart data={results.results}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis
                dataKey="time"
                tickLine={false}
                axisLine={false}
                tickMargin={8}
                tickFormatter={(value) => `${value}s`}
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
                dataKey="distanceTotal"
                stroke="var(--color-distanceTotal)"
                strokeWidth={2}
                dot={false}
              />
            </LineChart>
          </ChartContainer>
          <div className="flex justify-end gap-2 mt-4">
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
