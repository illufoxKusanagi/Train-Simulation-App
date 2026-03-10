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
  powerMotorOutputPerMotor: {
    label: "Power per Motor (kW)",
    color: "var(--chart-3)",
  },
} satisfies ChartConfig;

interface PowerPerMotorTabProps {
  results: SimulationResults;
  onDownloadCSV: (data: unknown[], filename: string) => void;
  onDownloadExcel: (data: unknown[], filename: string) => void;
}

export default function PowerPerMotorTab({
  results,
  onDownloadCSV,
  onDownloadExcel,
}: PowerPerMotorTabProps) {
  const exportData = (results.results || []).map((raw) => {
    const d = raw as unknown as Record<string, unknown>;
    return {
      phase: d.phase,
      iteration: d.iteration,
      time: d.time,
      timeTotal: d.timeTotal,
      speeds: d.speeds,
      powerMotorOutputPerMotor: d.powerMotorOutputPerMotor,
    };
  });

  return (
    <div className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>Power Output per Motor</CardTitle>
          <CardDescription>
            Motor power output per traction motor over time
          </CardDescription>
        </CardHeader>
        <CardContent>
          <ChartContainer
            config={chartConfig}
            className="aspect-auto h-[400px] w-full"
          >
            <LineChart data={results.results}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis
                dataKey="timeTotal"
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
                dataKey="powerMotorOutputPerMotor"
                stroke="var(--color-powerMotorOutputPerMotor)"
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
                onDownloadCSV(exportData, "power_per_motor_data.csv")
              }
            >
              <Download className="h-4 w-4 mr-2" />
              CSV
            </Button>
            <Button
              size="sm"
              variant="outline"
              onClick={() =>
                onDownloadExcel(exportData, "power_per_motor_data.xlsx")
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
