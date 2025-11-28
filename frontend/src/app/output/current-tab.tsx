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
  vvvfCurrents: {
    label: "VVVF Current (A)",
    color: "var(--chart-1)",
  },
  catenaryCurrents: {
    label: "Catenary Current (A)",
    color: "var(--chart-2)",
  },
} satisfies ChartConfig;

interface CurrentTabProps {
  results: SimulationResults;
  onDownloadCSV: (data: unknown[], filename: string) => void;
  onDownloadExcel: (data: unknown[], filename: string) => void;
}

export default function CurrentTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: CurrentTabProps) {
  // Validate data
  if (
    !results?.results ||
    !Array.isArray(results.results) ||
    results.results.length === 0
  ) {
    return (
      <div className="space-y-4">
        <Card>
          <CardHeader>
            <CardTitle>Current Draw</CardTitle>
            <CardDescription>
              VVVF and Catenary current over time
            </CardDescription>
          </CardHeader>
          <CardContent>
            <div className="flex items-center justify-center h-[400px] text-muted-foreground">
              No current data available
            </div>
          </CardContent>
        </Card>
      </div>
    );
  }

  return (
    <div className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>Current Draw</CardTitle>
          <CardDescription>VVVF and Catenary current over time</CardDescription>
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
            <Button
              size="sm"
              variant="outline"
              onClick={() => onDownloadCSV(results.results, "current_data.csv")}
            >
              <Download className="h-4 w-4 mr-2" />
              CSV
            </Button>
            <Button
              size="sm"
              onClick={() =>
                onDownloadExcel(results.results, "current_data.xlsx")
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
