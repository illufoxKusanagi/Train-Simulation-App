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
  vvvfPowers: {
    label: "VVVF Power (kW)",
    color: "var(--chart-1)",
  },
  catenaryPowers: {
    label: "Catenary Power (kW)",
    color: "var(--chart-2)",
  },
} satisfies ChartConfig;

interface PowerTabProps {
  results: SimulationResults;
  onDownloadCSV: (data: unknown[], filename: string) => void;
  onDownloadExcel: (data: unknown[], filename: string) => void;
}

export default function PowerTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: PowerTabProps) {
  return (
    <div className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>Power Consumption</CardTitle>
          <CardDescription>VVVF and Catenary power over time</CardDescription>
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
                tickFormatter={(value) => `${value} kW`}
              />
              <ChartTooltip cursor={false} content={<ChartTooltipContent />} />
              <ChartLegend content={<ChartLegendContent />} />
              <Line
                type="monotone"
                dataKey="vvvfPowers"
                stroke="var(--color-vvvfPowers)"
                strokeWidth={2}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="catenaryPowers"
                stroke="var(--color-catenaryPowers)"
                strokeWidth={2}
                dot={false}
              />
            </LineChart>
          </ChartContainer>
          <div className="flex justify-end gap-2 mt-4">
            <Button
              size="sm"
              variant="outline"
              onClick={() => onDownloadCSV(results.results, "power_data.csv")}
            >
              <Download className="h-4 w-4 mr-2" />
              CSV
            </Button>
            <Button
              size="sm"
              onClick={() =>
                onDownloadExcel(results.results, "power_data.xlsx")
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
