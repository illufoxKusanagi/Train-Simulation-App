import { useMemo } from "react";
import {
  ComposedChart,
  Scatter,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
  ReferenceLine,
  Area,
} from "recharts";
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
  CardDescription,
} from "@/components/ui/card";
import { Clock, Zap, Trophy, ArrowRight, BatteryCharging } from "lucide-react";

interface OptResult {
  acc_start_si: number;
  v_p1: number;
  peakMotorPower: number;
  travelTime: number;
  fuzzyScore: number;
  energyConsumption: number;
}

interface FuzzyMemberTabProps {
  results: OptResult[];
  best: OptResult;
}

// Trapezoidal membership function — identical to C++ TrapezoidSet::membership
const trap = (x: number, a: number, b: number, c: number, d: number) => {
  if (x <= a || x >= d) return 0;
  if (x >= b && x <= c) return 1;
  if (x < b) return (x - a) / (b - a);
  return (d - x) / (d - c);
};

// Triangle membership function — identical to C++ TriangleSet::membership
const tri = (x: number, a: number, b: number, c: number) => {
  if (x <= a || x >= c) return 0;
  if (x === b) return 1;
  if (x < b) return (x - a) / (b - a);
  return (c - x) / (c - b);
};

export function FuzzyMemberTab({ results, best }: FuzzyMemberTabProps) {
  const chartData = useMemo(() => {
    if (!results || results.length === 0)
      return {
        timeData: [],
        powerData: [],
        energyData: [],
        outputData: [],
        bestTimeMu: { Short: 0, Medium: 0, Long: 0 },
        bestPowerMu: { Low: 0, Medium: 0, High: 0 },
        bestEnergyMu: { Low: 0, Medium: 0, High: 0 },
        bestOutputMu: { Poor: 0, Fair: 0, Good: 0, Excellent: 0 },
        bounds: {
          effMinT: 0,
          effMaxT: 0,
          effMinP: 0,
          effMaxP: 0,
          effMinE: 0,
          effMaxE: 0,
        },
      };

    // ── Find absolute boundaries from sweep ──
    let minT = results[0].travelTime,
      maxT = results[0].travelTime;
    let minP = results[0].peakMotorPower,
      maxP = results[0].peakMotorPower;
    let minE = results[0].energyConsumption,
      maxE = results[0].energyConsumption;

    for (const r of results) {
      if (r.travelTime < minT) minT = r.travelTime;
      if (r.travelTime > maxT) maxT = r.travelTime;
      if (r.peakMotorPower < minP) minP = r.peakMotorPower;
      if (r.peakMotorPower > maxP) maxP = r.peakMotorPower;
      if (r.energyConsumption < minE) minE = r.energyConsumption;
      if (r.energyConsumption > maxE) maxE = r.energyConsumption;
    }

    // Backend degenerate guard
    if (maxT - minT < 1.0) {
      minT -= 1.0;
      maxT += 1.0;
    }
    if (maxP - minP < 1.0) {
      minP -= 1.0;
      maxP += 1.0;
    }
    if (maxE - minE < 1.0) {
      minE -= 1.0;
      maxE += 1.0;
    }

    // Backend 5% expansion margins
    const marginT = (maxT - minT) * 0.05;
    const effMinT = minT - marginT;
    const effMaxT = maxT + marginT;
    const rT = effMaxT - effMinT;

    const marginP = (maxP - minP) * 0.05;
    const effMinP = minP - marginP;
    const effMaxP = maxP + marginP;
    const rP = effMaxP - effMinP;

    const marginE = (maxE - minE) * 0.05;
    const effMinE = minE - marginE;
    const effMaxE = maxE + marginE;
    const rE = effMaxE - effMinE;

    // ── Travel Time Input Membership ──
    const steps = 150;
    const rawTimeData = [];
    for (let i = 0; i <= steps; i++) {
      const x = effMinT + (i * rT) / steps;
      rawTimeData.push({
        x: Number(x.toFixed(1)),
        Short: trap(
          x,
          effMinT,
          effMinT,
          effMinT + 0.25 * rT,
          effMinT + 0.45 * rT,
        ),
        Medium: tri(
          x,
          effMinT + 0.3 * rT,
          effMinT + 0.5 * rT,
          effMinT + 0.7 * rT,
        ),
        Long: trap(
          x,
          effMinT + 0.55 * rT,
          effMinT + 0.75 * rT,
          effMaxT,
          effMaxT,
        ),
      });
    }

    results.forEach((r) => {
      const x = Number(r.travelTime.toFixed(1));
      rawTimeData.push({
        x,
        Short: trap(
          x,
          effMinT,
          effMinT,
          effMinT + 0.25 * rT,
          effMinT + 0.45 * rT,
        ),
        Medium: tri(
          x,
          effMinT + 0.3 * rT,
          effMinT + 0.5 * rT,
          effMinT + 0.7 * rT,
        ),
        Long: trap(
          x,
          effMinT + 0.55 * rT,
          effMinT + 0.75 * rT,
          effMaxT,
          effMaxT,
        ),
        dot: 0.02,
        winnerDot:
          best && Math.abs(x - best.travelTime) < 0.01 ? 0.02 : undefined,
      });
    });

    const timeData = rawTimeData.sort((a, b) => a.x - b.x);

    // ── Motor Power Input Membership ──
    const rawPowerData = [];
    for (let i = 0; i <= steps; i++) {
      const x = effMinP + (i * rP) / steps;
      rawPowerData.push({
        x: Number(x.toFixed(1)),
        Low: trap(
          x,
          effMinP,
          effMinP,
          effMinP + 0.25 * rP,
          effMinP + 0.45 * rP,
        ),
        Medium: tri(
          x,
          effMinP + 0.3 * rP,
          effMinP + 0.5 * rP,
          effMinP + 0.7 * rP,
        ),
        High: trap(
          x,
          effMinP + 0.55 * rP,
          effMinP + 0.75 * rP,
          effMaxP,
          effMaxP,
        ),
      });
    }

    results.forEach((r) => {
      const x = Number(r.peakMotorPower.toFixed(1));
      rawPowerData.push({
        x,
        Low: trap(
          x,
          effMinP,
          effMinP,
          effMinP + 0.25 * rP,
          effMinP + 0.45 * rP,
        ),
        Medium: tri(
          x,
          effMinP + 0.3 * rP,
          effMinP + 0.5 * rP,
          effMinP + 0.7 * rP,
        ),
        High: trap(
          x,
          effMinP + 0.55 * rP,
          effMinP + 0.75 * rP,
          effMaxP,
          effMaxP,
        ),
        dot: 0.02,
        winnerDot:
          best && Math.abs(x - best.peakMotorPower) < 0.01 ? 0.02 : undefined,
      });
    });

    const powerData = rawPowerData.sort((a, b) => a.x - b.x);

    // ── Energy Consumption Input Membership ──
    const rawEnergyData = [];
    for (let i = 0; i <= steps; i++) {
      const x = effMinE + (i * rE) / steps;
      rawEnergyData.push({
        x: Number(x.toFixed(2)),
        Low: trap(x, effMinE, effMinE, effMinE + 0.25 * rE, effMinE + 0.45 * rE),
        Medium: tri(x, effMinE + 0.3 * rE, effMinE + 0.5 * rE, effMinE + 0.7 * rE),
        High: trap(x, effMinE + 0.55 * rE, effMinE + 0.75 * rE, effMaxE, effMaxE),
      });
    }

    results.forEach((r) => {
      const x = Number(r.energyConsumption.toFixed(2));
      rawEnergyData.push({
        x,
        Low: trap(x, effMinE, effMinE, effMinE + 0.25 * rE, effMinE + 0.45 * rE),
        Medium: tri(x, effMinE + 0.3 * rE, effMinE + 0.5 * rE, effMinE + 0.7 * rE),
        High: trap(x, effMinE + 0.55 * rE, effMinE + 0.75 * rE, effMaxE, effMaxE),
        dot: 0.02,
        winnerDot:
          best && Math.abs(x - best.energyConsumption) < 0.001 ? 0.02 : undefined,
      });
    });

    const energyData = rawEnergyData.sort((a, b) => a.x - b.x);

    // ── Output Membership (Score 0-100) — same for both engines ──
    const rawOutputData = [];
    for (let i = 0; i <= 100; i++) {
      const x = i;
      rawOutputData.push({
        x,
        Poor: trap(x, 0, 0, 15, 30),
        Fair: tri(x, 20, 38, 55),
        Good: tri(x, 45, 62, 78),
        Excellent: trap(x, 68, 82, 100, 100),
      });
    }

    results.forEach((r) => {
      const x = Number(r.fuzzyScore.toFixed(1));
      rawOutputData.push({
        x,
        Poor: trap(x, 0, 0, 15, 30),
        Fair: tri(x, 20, 38, 55),
        Good: tri(x, 45, 62, 78),
        Excellent: trap(x, 68, 82, 100, 100),
        dot: 0.02,
        winnerDot:
          best && Math.abs(x - best.fuzzyScore) < 0.01 ? 0.02 : undefined,
      });
    });

    const outputData = rawOutputData.sort((a, b) => a.x - b.x);

    // ── Compute best result's membership degrees ──
    const bT = best.travelTime;
    const bestTimeMu = {
      Short: trap(
        bT,
        effMinT,
        effMinT,
        effMinT + 0.25 * rT,
        effMinT + 0.45 * rT,
      ),
      Medium: tri(
        bT,
        effMinT + 0.3 * rT,
        effMinT + 0.5 * rT,
        effMinT + 0.7 * rT,
      ),
      Long: trap(
        bT,
        effMinT + 0.55 * rT,
        effMinT + 0.75 * rT,
        effMaxT,
        effMaxT,
      ),
    };

    const bP = best.peakMotorPower;
    const bestPowerMu = {
      Low: trap(bP, effMinP, effMinP, effMinP + 0.25 * rP, effMinP + 0.45 * rP),
      Medium: tri(bP, effMinP + 0.3 * rP, effMinP + 0.5 * rP, effMinP + 0.7 * rP),
      High: trap(bP, effMinP + 0.55 * rP, effMinP + 0.75 * rP, effMaxP, effMaxP),
    };

    const bE = best.energyConsumption;
    const bestEnergyMu = {
      Low: trap(bE, effMinE, effMinE, effMinE + 0.25 * rE, effMinE + 0.45 * rE),
      Medium: tri(bE, effMinE + 0.3 * rE, effMinE + 0.5 * rE, effMinE + 0.7 * rE),
      High: trap(bE, effMinE + 0.55 * rE, effMinE + 0.75 * rE, effMaxE, effMaxE),
    };

    const bS = best.fuzzyScore;
    const bestOutputMu = {
      Poor: trap(bS, 0, 0, 15, 30),
      Fair: tri(bS, 20, 38, 55),
      Good: tri(bS, 45, 62, 78),
      Excellent: trap(bS, 68, 82, 100, 100),
    };

    return {
      timeData,
      powerData,
      energyData,
      outputData,
      bestTimeMu,
      bestPowerMu,
      bestEnergyMu,
      bestOutputMu,
      bounds: { effMinT, effMaxT, effMinP, effMaxP, effMinE, effMaxE },
    };
  }, [results, best]);

  if (chartData.timeData.length === 0) return null;

  // Find the dominant membership category for the best result
  const dominantTime = Object.entries(chartData.bestTimeMu).sort(
    (a, b) => b[1] - a[1],
  )[0];
  const dominantPower = Object.entries(chartData.bestPowerMu).sort(
    (a, b) => b[1] - a[1],
  )[0];
  const dominantEnergy = Object.entries(chartData.bestEnergyMu).sort(
    (a, b) => b[1] - a[1],
  )[0];
  const dominantOutput = Object.entries(chartData.bestOutputMu).sort(
    (a, b) => b[1] - a[1],
  )[0];

  return (
    <div className="flex flex-col gap-4 mt-4">
      {/* Summary Insight Card */}
      <Card className="border-primary/20 bg-primary/5">
        <CardContent className="pt-6">
          <div className="flex flex-col md:flex-row items-center justify-center gap-3 md:gap-6 text-sm">
            <div className="flex items-center gap-2">
              <Trophy className="h-5 w-5 text-yellow-500" />
              <span className="font-semibold">Winner Analysis:</span>
            </div>
            <div className="flex items-center gap-2 flex-wrap justify-center">
              <span className="px-2 py-1 rounded bg-secondary text-xs font-mono">
                Time = {best.travelTime.toFixed(1)}s
              </span>
              <ArrowRight className="h-3 w-3 text-muted-foreground" />
              <span className="px-2 py-1 rounded bg-green-100 dark:bg-green-900 text-xs font-semibold text-green-700 dark:text-green-300">
                μ({dominantTime[0]}) = {dominantTime[1].toFixed(3)}
              </span>
              <span className="text-muted-foreground mx-1">|</span>
              <span className="px-2 py-1 rounded bg-secondary text-xs font-mono">
                Power = {best.peakMotorPower.toFixed(1)}kW
              </span>
              <ArrowRight className="h-3 w-3 text-muted-foreground" />
              <span className="px-2 py-1 rounded bg-blue-100 dark:bg-blue-900 text-xs font-semibold text-blue-700 dark:text-blue-300">
                μ({dominantPower[0]}) = {dominantPower[1].toFixed(3)}
              </span>
              <span className="text-muted-foreground mx-1">|</span>
              <span className="px-2 py-1 rounded bg-secondary text-xs font-mono">
                Energy = {best.energyConsumption.toFixed(2)}kWh
              </span>
              <ArrowRight className="h-3 w-3 text-muted-foreground" />
              <span className="px-2 py-1 rounded bg-purple-100 dark:bg-purple-900 text-xs font-semibold text-purple-700 dark:text-purple-300">
                μ({dominantEnergy[0]}) = {dominantEnergy[1].toFixed(3)}
              </span>
              <span className="text-muted-foreground mx-1">|</span>
              <span className="px-2 py-1 rounded bg-secondary text-xs font-mono">
                Score = {best.fuzzyScore.toFixed(1)}
              </span>
              <ArrowRight className="h-3 w-3 text-muted-foreground" />
              <span className="px-2 py-1 rounded bg-yellow-100 dark:bg-yellow-900 text-xs font-semibold text-yellow-700 dark:text-yellow-300">
                μ({dominantOutput[0]}) = {dominantOutput[1].toFixed(3)}
              </span>
            </div>
          </div>
        </CardContent>
      </Card>

      {/* Input Membership Charts */}
      <div className="grid grid-cols-1 xl:grid-cols-2 gap-4">
        {/* Travel Time */}
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-lg flex items-center gap-2">
              <Clock className="h-5 w-5 text-green-500" />
              Travel Time Membership
            </CardTitle>
            <CardDescription>
              Input variable: Short (fast) → Long (slow). Range:{" "}
              {chartData.bounds.effMinT.toFixed(0)}s –{" "}
              {chartData.bounds.effMaxT.toFixed(0)}s
            </CardDescription>
          </CardHeader>
          <CardContent className="h-72 w-full">
            <ResponsiveContainer width="100%" height="100%">
              <ComposedChart
                data={chartData.timeData}
                margin={{ top: 35, right: 20, left: -10, bottom: 5 }}
              >
                <CartesianGrid strokeDasharray="3 3" opacity={0.2} />
                <XAxis
                  dataKey="x"
                  type="number"
                  domain={["dataMin", "dataMax"]}
                  tickCount={8}
                  tick={{ fontSize: 11 }}
                  tickFormatter={(v) => Math.round(v).toString()}
                  label={{
                    value: "Travel Time (s)",
                    position: "insideBottom",
                    offset: -2,
                    fontSize: 11,
                  }}
                />
                <YAxis
                  tick={{ fontSize: 11 }}
                  domain={[0, 1.05]}
                  label={{
                    value: "μ(x)",
                    angle: -90,
                    position: "insideLeft",
                    offset: 20,
                    fontSize: 11,
                  }}
                />
                <Tooltip
                  contentStyle={{
                    backgroundColor: "var(--card)",
                    borderColor: "var(--border)",
                    color: "var(--foreground)",
                  }}
                  labelStyle={{ color: "var(--foreground)" }}
                  labelFormatter={(v) => `Time: ${v}s`}
                  formatter={(value: number, name: string) => {
                    if (name === "Winner" || name === "All Results") return [];
                    return [value.toFixed(3), `μ(${name})`];
                  }}
                />
                <Legend />

                <Area
                  type="linear"
                  dataKey="Short"
                  stroke="#22c55e"
                  fill="#22c55e"
                  fillOpacity={0.1}
                  strokeWidth={2.5}
                  dot={false}
                />
                <Area
                  type="linear"
                  dataKey="Medium"
                  stroke="#eab308"
                  fill="#eab308"
                  fillOpacity={0.1}
                  strokeWidth={2.5}
                  dot={false}
                />
                <Area
                  type="linear"
                  dataKey="Long"
                  stroke="#ef4444"
                  fill="#ef4444"
                  fillOpacity={0.1}
                  strokeWidth={2.5}
                  dot={false}
                />

                {/* Dots embedded directly into area data */}
                <Scatter
                  dataKey="dot"
                  fill="var(--muted-foreground)"
                  opacity={0.4}
                  r={3}
                  name="All Results"
                />
                <Scatter
                  dataKey="winnerDot"
                  fill="var(--primary)"
                  shape="star"
                  r={8}
                  name="Winner"
                />

                {/* Best result vertical line */}
                <ReferenceLine
                  x={Number(best.travelTime.toFixed(1))}
                  stroke="var(--primary)"
                  isFront={true}
                  strokeWidth={2}
                  strokeDasharray="6 3"
                  label={{
                    value: `★ ${best.travelTime.toFixed(0)}s`,
                    fill: "var(--primary)",
                    position: "top",
                    fontSize: 14,
                    fontWeight: "bold",
                  }}
                />
              </ComposedChart>
            </ResponsiveContainer>
          </CardContent>
          {/* Membership degree badges */}
          <div className="px-6 pb-4 flex gap-2 flex-wrap">
            <span className="text-xs text-muted-foreground">
              Winner membership:
            </span>
            {Object.entries(chartData.bestTimeMu).map(([name, val]) => (
              <span
                key={name}
                className={`text-xs px-2 py-0.5 rounded-full font-mono ${
                  val > 0
                    ? "bg-primary/10 text-primary font-semibold"
                    : "bg-muted text-muted-foreground"
                }`}
              >
                {name}: {val.toFixed(3)}
              </span>
            ))}
          </div>
        </Card>

        {/* Peak Motor Power */}
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-lg flex items-center gap-2">
              <Zap className="h-5 w-5 text-yellow-500" />
              Peak Motor Power Membership
            </CardTitle>
            <CardDescription>
              Input variable: Low (efficient) → High (hungry). Range:{" "}
              {chartData.bounds.effMinP.toFixed(1)}kW –{" "}
              {chartData.bounds.effMaxP.toFixed(1)}kW
            </CardDescription>
          </CardHeader>
          <CardContent className="h-72 w-full">
            <ResponsiveContainer width="100%" height="100%">
              <ComposedChart
                data={chartData.powerData}
                margin={{ top: 35, right: 20, left: -10, bottom: 5 }}
              >
                <CartesianGrid strokeDasharray="3 3" opacity={0.2} />
                <XAxis
                  dataKey="x"
                  type="number"
                  domain={["dataMin", "dataMax"]}
                  tickCount={8}
                  tick={{ fontSize: 11 }}
                  tickFormatter={(v) => Math.round(v).toString()}
                  label={{
                    value: "Peak Power (kW)",
                    position: "insideBottom",
                    offset: -2,
                    fontSize: 11,
                  }}
                />
                <YAxis
                  tick={{ fontSize: 11 }}
                  domain={[0, 1.05]}
                  label={{
                    value: "μ(x)",
                    angle: -90,
                    position: "insideLeft",
                    offset: 20,
                    fontSize: 11,
                  }}
                />
                <Tooltip
                  contentStyle={{
                    backgroundColor: "var(--card)",
                    borderColor: "var(--border)",
                    color: "var(--foreground)",
                  }}
                  labelStyle={{ color: "var(--foreground)" }}
                  labelFormatter={(v) => `Power: ${v}kW`}
                  formatter={(value: number, name: string) => {
                    if (name === "Winner" || name === "All Results") return [];
                    return [value.toFixed(3), `μ(${name})`];
                  }}
                />
                <Legend />

                <Area
                  type="linear"
                  dataKey="Low"
                  stroke="#22c55e"
                  fill="#22c55e"
                  fillOpacity={0.1}
                  strokeWidth={2.5}
                  dot={false}
                />
                <Area
                  type="linear"
                  dataKey="Medium"
                  stroke="#eab308"
                  fill="#eab308"
                  fillOpacity={0.1}
                  strokeWidth={2.5}
                  dot={false}
                />
                <Area
                  type="linear"
                  dataKey="High"
                  stroke="#ef4444"
                  fill="#ef4444"
                  fillOpacity={0.1}
                  strokeWidth={2.5}
                  dot={false}
                />

                {/* Dots embedded directly into area data */}
                <Scatter
                  dataKey="dot"
                  fill="var(--muted-foreground)"
                  opacity={0.4}
                  r={3}
                  name="All Results"
                />
                <Scatter
                  dataKey="winnerDot"
                  fill="var(--primary)"
                  shape="star"
                  r={8}
                  name="Winner"
                />

                {/* Best result vertical line */}
                <ReferenceLine
                  x={Number(best.peakMotorPower.toFixed(1))}
                  stroke="var(--primary)"
                  isFront={true}
                  strokeWidth={2}
                  strokeDasharray="6 3"
                  label={{
                    value: `★ ${best.peakMotorPower.toFixed(1)}kW`,
                    fill: "var(--primary)",
                    position: "top",
                    fontSize: 14,
                    fontWeight: "bold",
                  }}
                />
              </ComposedChart>
            </ResponsiveContainer>
          </CardContent>
          {/* Membership degree badges */}
          <div className="px-6 pb-4 flex gap-2 flex-wrap">
            <span className="text-xs text-muted-foreground">
              Winner membership:
            </span>
            {Object.entries(chartData.bestPowerMu).map(([name, val]) => (
              <span
                key={name}
                className={`text-xs px-2 py-0.5 rounded-full font-mono ${
                  val > 0
                    ? "bg-primary/10 text-primary font-semibold"
                    : "bg-muted text-muted-foreground"
                }`}
              >
                {name}: {val.toFixed(3)}
              </span>
            ))}
          </div>
        </Card>
      </div>

      {/* Energy Consumption Membership */}
      <Card>
        <CardHeader className="pb-2">
          <CardTitle className="text-lg flex items-center gap-2">
            <BatteryCharging className="h-5 w-5 text-purple-500" />
            Energy Consumption Membership
          </CardTitle>
          <CardDescription>
            Input variable: Low (efficient) → High (hungry). Range:{" "}
            {chartData.bounds.effMinE.toFixed(2)}kWh –{" "}
            {chartData.bounds.effMaxE.toFixed(2)}kWh
          </CardDescription>
        </CardHeader>
        <CardContent className="h-72 w-full">
          <ResponsiveContainer width="100%" height="100%">
            <ComposedChart
              data={chartData.energyData}
              margin={{ top: 35, right: 20, left: -10, bottom: 5 }}
            >
              <CartesianGrid strokeDasharray="3 3" opacity={0.2} />
              <XAxis
                dataKey="x"
                type="number"
                domain={["dataMin", "dataMax"]}
                tickCount={8}
                tick={{ fontSize: 11 }}
                tickFormatter={(v) => v.toFixed(1)}
                label={{
                  value: "Energy Consumption (kWh)",
                  position: "insideBottom",
                  offset: -2,
                  fontSize: 11,
                }}
              />
              <YAxis
                tick={{ fontSize: 11 }}
                domain={[0, 1.05]}
                label={{
                  value: "μ(x)",
                  angle: -90,
                  position: "insideLeft",
                  offset: 20,
                  fontSize: 11,
                }}
              />
              <Tooltip
                contentStyle={{
                  backgroundColor: "var(--card)",
                  borderColor: "var(--border)",
                  color: "var(--foreground)",
                }}
                labelStyle={{ color: "var(--foreground)" }}
                labelFormatter={(v) => `Energy: ${v}kWh`}
                formatter={(value: number, name: string) => {
                  if (name === "Winner" || name === "All Results") return [];
                  return [value.toFixed(3), `μ(${name})`];
                }}
              />
              <Legend />
              <Area
                type="linear"
                dataKey="Low"
                stroke="#22c55e"
                fill="#22c55e"
                fillOpacity={0.1}
                strokeWidth={2.5}
                dot={false}
              />
              <Area
                type="linear"
                dataKey="Medium"
                stroke="#a855f7"
                fill="#a855f7"
                fillOpacity={0.1}
                strokeWidth={2.5}
                dot={false}
              />
              <Area
                type="linear"
                dataKey="High"
                stroke="#ef4444"
                fill="#ef4444"
                fillOpacity={0.1}
                strokeWidth={2.5}
                dot={false}
              />
              <Scatter
                dataKey="dot"
                fill="var(--muted-foreground)"
                opacity={0.4}
                r={3}
                name="All Results"
              />
              <Scatter
                dataKey="winnerDot"
                fill="var(--primary)"
                shape="star"
                r={8}
                name="Winner"
              />
              <ReferenceLine
                x={Number(best.energyConsumption.toFixed(2))}
                stroke="var(--primary)"
                isFront={true}
                strokeWidth={2}
                strokeDasharray="6 3"
                label={{
                  value: `★ ${best.energyConsumption.toFixed(2)}kWh`,
                  fill: "var(--primary)",
                  position: "top",
                  fontSize: 14,
                  fontWeight: "bold",
                }}
              />
            </ComposedChart>
          </ResponsiveContainer>
        </CardContent>
        <div className="px-6 pb-4 flex gap-2 flex-wrap">
          <span className="text-xs text-muted-foreground">Winner membership:</span>
          {Object.entries(chartData.bestEnergyMu).map(([name, val]) => (
            <span
              key={name}
              className={`text-xs px-2 py-0.5 rounded-full font-mono ${
                val > 0
                  ? "bg-purple-100 dark:bg-purple-900 text-purple-700 dark:text-purple-300 font-semibold"
                  : "bg-muted text-muted-foreground"
              }`}
            >
              {name}: {val.toFixed(3)}
            </span>
          ))}
        </div>
      </Card>

      {/* Output Membership Chart (Score 0-100) */}
      <Card>
        <CardHeader className="pb-2">
          <CardTitle className="text-lg flex items-center gap-2">
            <Trophy className="h-5 w-5 text-primary" />
            Output Membership — Optimization Score
          </CardTitle>
          <CardDescription>
            Defuzzified output (Centroid) from Time, Power, and Energy engines.
            The final score is the average of TimeScore, PowerScore, and EnergyScore.
          </CardDescription>
        </CardHeader>
        <CardContent className="h-72 w-full">
          <ResponsiveContainer width="100%" height="100%">
            <ComposedChart
              data={chartData.outputData}
              margin={{ top: 35, right: 20, left: -10, bottom: 5 }}
            >
              <CartesianGrid strokeDasharray="3 3" opacity={0.2} />
              <XAxis
                dataKey="x"
                type="number"
                domain={[0, 100]}
                tickCount={11}
                tick={{ fontSize: 11 }}
                label={{
                  value: "Score (0–100)",
                  position: "insideBottom",
                  offset: -2,
                  fontSize: 11,
                }}
              />
              <YAxis
                tick={{ fontSize: 11 }}
                domain={[0, 1.05]}
                label={{
                  value: "μ(x)",
                  angle: -90,
                  position: "insideLeft",
                  offset: 20,
                  fontSize: 11,
                }}
              />
              <Tooltip
                contentStyle={{
                  backgroundColor: "var(--card)",
                  borderColor: "var(--border)",
                  color: "var(--foreground)",
                }}
                labelStyle={{ color: "var(--foreground)" }}
                labelFormatter={(v) => `Score: ${v}`}
                formatter={(value: number, name: string) => {
                  if (name === "Winner Score" || name === "All Scores")
                    return [];
                  return [value.toFixed(3), `μ(${name})`];
                }}
              />
              <Legend />

              <Area
                type="linear"
                dataKey="Poor"
                stroke="#ef4444"
                fill="#ef4444"
                fillOpacity={0.1}
                strokeWidth={2.5}
                dot={false}
              />
              <Area
                type="linear"
                dataKey="Fair"
                stroke="#f97316"
                fill="#f97316"
                fillOpacity={0.1}
                strokeWidth={2.5}
                dot={false}
              />
              <Area
                type="linear"
                dataKey="Good"
                stroke="#eab308"
                fill="#eab308"
                fillOpacity={0.1}
                strokeWidth={2.5}
                dot={false}
              />
              <Area
                type="linear"
                dataKey="Excellent"
                stroke="#22c55e"
                fill="#22c55e"
                fillOpacity={0.1}
                strokeWidth={2.5}
                dot={false}
              />

              <Scatter
                dataKey="dot"
                fill="var(--muted-foreground)"
                opacity={0.4}
                r={3}
                name="All Scores"
              />
              <Scatter
                dataKey="winnerDot"
                fill="var(--primary)"
                shape="star"
                r={8}
                name="Winner Score"
              />

              {/* Best result vertical line */}
              <ReferenceLine
                x={Number(best.fuzzyScore.toFixed(1))}
                stroke="var(--primary)"
                isFront={true}
                strokeWidth={2}
                strokeDasharray="6 3"
                label={{
                  value: `★ Score: ${best.fuzzyScore.toFixed(1)}`,
                  fill: "var(--primary)",
                  position: "top",
                  fontSize: 14,
                  fontWeight: "bold",
                }}
              />
            </ComposedChart>
          </ResponsiveContainer>
        </CardContent>
        {/* Output membership degree badges */}
        <div className="px-6 pb-4 flex gap-2 flex-wrap">
          <span className="text-xs text-muted-foreground">
            Winner output membership:
          </span>
          {Object.entries(chartData.bestOutputMu).map(([name, val]) => (
            <span
              key={name}
              className={`text-xs px-2 py-0.5 rounded-full font-mono ${
                val > 0
                  ? "bg-primary/10 text-primary font-semibold"
                  : "bg-muted text-muted-foreground"
              }`}
            >
              {name}: {val.toFixed(3)}
            </span>
          ))}
        </div>
      </Card>
    </div>
  );
}
