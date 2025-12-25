"use client";

import { useState, useEffect, useRef } from "react";
import { api } from "@/services/api";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  ReferenceLine,
} from "recharts";
import { toast } from "sonner";
import {
  Play,
  Square,
  Check,
  Loader2,
  TrendingUp,
  Zap,
  Clock,
} from "lucide-react";
import PageLayout from "@/components/page-layout";

export default function OptimizationPage() {
  const [isRunning, setIsRunning] = useState(false);
  const [iteration, setIteration] = useState(0);
  const [score, setScore] = useState(0);
  const [label, setLabel] = useState("Not Started");
  const [history, setHistory] = useState<{ iter: number; score: number }[]>([]);
  const [bestParams, setBestParams] = useState<Record<string, number> | null>(
    null
  );
  const [metrics, setMetrics] = useState<{
    time: number;
    powerPerMotor: number;
  }>({ time: 0, powerPerMotor: 0 });

  const pollingRef = useRef<NodeJS.Timeout | null>(null);

  const startOptimization = async () => {
    try {
      setIsRunning(true);
      setHistory([]);
      await api.startOptimization();
      toast.success("Optimization started");
      startPolling();
    } catch (error) {
      console.error(error);
      toast.error("Failed to start optimization");
      setIsRunning(false);
    }
  };

  const stopOptimization = async () => {
    try {
      await api.stopOptimization();
      toast.info("Stopping optimization...");
    } catch (error) {
      console.error(error);
      toast.error("Failed to stop optimization");
    }
  };

  const applyOptimization = async () => {
    try {
      await api.applyOptimization();
      toast.success("Optimization results applied to train parameters!");
    } catch (error) {
      console.error(error);
      toast.error("Failed to apply results");
    }
  };

  const startPolling = () => {
    if (pollingRef.current) clearInterval(pollingRef.current);
    pollingRef.current = setInterval(async () => {
      try {
        const status = await api.getOptimizationStatus();
        setIsRunning(status.isRunning);
        setIteration(status.iteration);
        setScore(status.suitabilityScore);
        setLabel(status.suitabilityLabel);
        setBestParams(status.optimizedTrain);

        if (status.travelTime !== undefined) {
          // Use actual metrics from backend
          setMetrics({
            time: status.travelTime || 0,
            powerPerMotor: status.maxPowerMotorPerMotor || 0,
          });
        } else if (status.debug_speed !== undefined) {
          // Fallback to debug metrics if new fields missing (should not happen after update)
          setMetrics({
            time: status.debug_acc || 0,
            powerPerMotor: status.debug_wp || 0,
          });
        }

        // Update chart history
        const newHistory = status.scoreHistory.map((s, i) => ({
          iter: i + 1,
          score: s,
        }));
        setHistory(newHistory);

        if (!status.isRunning) {
          if (pollingRef.current) clearInterval(pollingRef.current);
          toast.success("Optimization finished!");
        }
      } catch (error) {
        console.error("Polling error", error);
      }
    }, 500);
  };

  useEffect(() => {
    return () => {
      if (pollingRef.current) clearInterval(pollingRef.current);
    };
  }, []);

  return (
    <PageLayout>
      <div className="container mx-auto p-6 space-y-6">
        <div className="flex justify-between items-center">
          <div>
            <h1 className="text-3xl font-bold tracking-tight">
              Fuzzy Optimization
            </h1>
            <p className="text-muted-foreground">
              Optimize train parameters for best Travel Time and Energy
              Efficiency.
            </p>
          </div>
          <div className="flex gap-2">
            {!isRunning ? (
              <Button
                onClick={startOptimization}
                className="bg-green-600 hover:bg-green-700"
              >
                <Play className="mr-2 h-4 w-4" /> Start Optimization
              </Button>
            ) : (
              <Button onClick={stopOptimization} variant="destructive">
                <Square className="mr-2 h-4 w-4" /> Stop
              </Button>
            )}
            <Button
              onClick={applyOptimization}
              disabled={isRunning || !bestParams}
              variant="secondary"
            >
              <Check className="mr-2 h-4 w-4" /> Apply Results
            </Button>
          </div>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          {/* Status Card */}
          <Card>
            <CardHeader>
              <CardTitle>Current Status</CardTitle>
            </CardHeader>
            <CardContent className="space-y-4">
              <div className="flex justify-between items-center">
                <span className="text-sm font-medium">Iteration</span>
                <span className="text-2xl font-bold">{iteration}</span>
              </div>
              <div className="flex justify-between items-center">
                <span className="text-sm font-medium">Suitability Score</span>
                <span
                  className={`text-2xl font-bold ${
                    score > 0.8
                      ? "text-green-500"
                      : score > 0.5
                      ? "text-yellow-500"
                      : "text-red-500"
                  }`}
                >
                  {score.toFixed(4)}
                </span>
              </div>
              <div className="flex justify-between items-center">
                <span className="text-sm font-medium">Verdict</span>
                <span className="px-2 py-1 rounded bg-secondary text-secondary-foreground text-sm font-semibold">
                  {label}
                </span>
              </div>
              {isRunning && (
                <div className="flex items-center justify-center py-4">
                  <Loader2 className="h-8 w-8 animate-spin text-primary" />
                </div>
              )}
            </CardContent>
          </Card>

          {/* Metrics Card */}
          <Card>
            <CardHeader>
              <CardTitle>Best Candidate Metrics</CardTitle>
            </CardHeader>
            <CardContent className="space-y-4">
              <div className="flex items-center gap-4">
                <div className="p-2 bg-blue-100 dark:bg-blue-900 rounded-full">
                  <Clock className="h-6 w-6 text-blue-600 dark:text-blue-400" />
                </div>
                <div>
                  <p className="text-sm text-muted-foreground">Travel Time</p>
                  <p className="text-xl font-bold">
                    {metrics.time > 0 ? metrics.time.toFixed(1) : "---"} s
                  </p>
                </div>
              </div>
              <div className="flex items-center gap-4">
                <div className="p-2 bg-yellow-100 dark:bg-yellow-900 rounded-full">
                  <Zap className="h-6 w-6 text-yellow-600 dark:text-yellow-400" />
                </div>
                <div>
                  <p className="text-sm text-muted-foreground">
                    Power per Motor
                  </p>
                  <p className="text-xl font-bold">
                    {metrics.powerPerMotor > 0
                      ? metrics.powerPerMotor.toFixed(2)
                      : "---"}{" "}
                    kW
                  </p>
                </div>
              </div>
            </CardContent>
          </Card>

          {/* Parameters Card */}
          <Card>
            <CardHeader>
              <CardTitle>Optimized Parameters</CardTitle>
            </CardHeader>
            <CardContent>
              {bestParams ? (
                <div className="space-y-2">
                  <div className="flex justify-between border-b pb-2">
                    <span>Gear Ratio</span>
                    <span className="font-mono font-bold">
                      {bestParams.gearRatio.toFixed(2)}
                    </span>
                  </div>
                  <div className="flex justify-between border-b pb-2">
                    <span>Motor Count (n_tm)</span>
                    <span className="font-mono font-bold">
                      {bestParams.n_tm}
                    </span>
                  </div>
                  {/* Add more params if optimized */}
                </div>
              ) : (
                <div className="text-center text-muted-foreground py-8">
                  No optimization results yet.
                </div>
              )}
            </CardContent>
          </Card>
        </div>

        {/* Chart */}
        <Card className="col-span-1 md:col-span-3">
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <TrendingUp className="h-5 w-5" /> Optimization Progress
            </CardTitle>
          </CardHeader>
          <CardContent className="h-[400px]">
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={history}>
                <CartesianGrid strokeDasharray="3 3" opacity={0.2} />
                <XAxis
                  dataKey="iter"
                  label={{
                    value: "Iteration",
                    position: "insideBottomRight",
                    offset: -5,
                  }}
                />
                <YAxis
                  domain={[0, 1]}
                  label={{
                    value: "Suitability Score",
                    angle: -90,
                    position: "insideLeft",
                  }}
                />
                <Tooltip
                  contentStyle={{
                    backgroundColor: "hsl(var(--card))",
                    borderColor: "hsl(var(--border))",
                  }}
                  itemStyle={{ color: "hsl(var(--foreground))" }}
                />
                <ReferenceLine
                  y={0.85}
                  label="Target"
                  stroke="green"
                  strokeDasharray="3 3"
                />
                <Line
                  type="monotone"
                  dataKey="score"
                  stroke="hsl(var(--primary))"
                  strokeWidth={3}
                  dot={false}
                  activeDot={{ r: 8 }}
                />
              </LineChart>
            </ResponsiveContainer>
          </CardContent>
        </Card>
      </div>
    </PageLayout>
  );
}
