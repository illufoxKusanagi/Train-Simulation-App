import { Trophy, Activity, Gauge, Zap, Clock } from "lucide-react";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";

interface OptResult {
  acc_start_si: number;
  v_p1: number;
  peakMotorPower: number;
  travelTime: number;
  fuzzyScore: number;
}

interface WinnerTabProps {
  best: OptResult;
  t: (key: string, values?: Record<string, string | number>) => string;
}

export function WinnerTab({ best, t }: WinnerTabProps) {
  const scoreColor = (score: number): string => {
    if (score >= 75) return "text-green-500";
    if (score >= 50) return "text-yellow-500";
    if (score >= 25) return "text-orange-500";
    return "text-red-500";
  };

  const scoreLabel = (score: number): string => {
    if (score >= 75) return t("excellent");
    if (score >= 50) return t("good");
    if (score >= 25) return t("fair");
    return t("poor");
  };

  const scoreBadgeClass = (score: number): string => {
    if (score >= 75)
      return "bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200";
    if (score >= 50)
      return "bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200";
    if (score >= 25)
      return "bg-orange-100 text-orange-800 dark:bg-orange-900 dark:text-orange-200";
    return "bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200";
  };

  return (
    <Card className="border-2 border-yellow-400 dark:border-yellow-500 mt-4">
      <CardHeader>
        <CardTitle className="flex items-center gap-2 text-yellow-600 dark:text-yellow-400">
          <Trophy className="h-6 w-6" />
          {t("bestCombination")}
        </CardTitle>
      </CardHeader>
      <CardContent>
        <div className="grid grid-cols-2 md:grid-cols-5 gap-4">
          <div className="flex flex-col items-center p-4 bg-secondary rounded-lg col-span-2 md:col-span-1">
            <Activity className="h-6 w-6 mb-2 text-primary" />
            <p className="text-xs text-muted-foreground">{t("fuzzyScore")}</p>
            <p className={`text-3xl font-black ${scoreColor(best.fuzzyScore)}`}>
              {best.fuzzyScore.toFixed(1)}
            </p>
            <span
              className={`mt-1 text-xs font-semibold px-2 py-0.5 rounded-full ${scoreBadgeClass(
                best.fuzzyScore,
              )}`}
            >
              {scoreLabel(best.fuzzyScore)}
            </span>
          </div>

          <div className="flex flex-col items-center p-4 bg-secondary rounded-lg">
            <Gauge className="h-6 w-6 mb-2 text-blue-500" />
            <p className="text-xs text-muted-foreground">{t("accelStart")}</p>
            <p className="text-2xl font-bold">{best.acc_start_si.toFixed(2)}</p>
            <p className="text-xs text-muted-foreground">m/s²</p>
          </div>

          <div className="flex flex-col items-center p-4 bg-secondary rounded-lg">
            <Activity className="h-6 w-6 mb-2 text-purple-500" />
            <p className="text-xs text-muted-foreground">{t("vp1FwStart")}</p>
            <p className="text-2xl font-bold">{best.v_p1.toFixed(1)}</p>
            <p className="text-xs text-muted-foreground">km/h</p>
          </div>

          <div className="flex flex-col items-center p-4 bg-secondary rounded-lg">
            <Zap className="h-6 w-6 mb-2 text-yellow-500" />
            <p className="text-xs text-muted-foreground">
              {t("peakPowerMotor")}
            </p>
            <p className="text-2xl font-bold">
              {best.peakMotorPower.toFixed(1)}
            </p>
            <p className="text-xs text-muted-foreground">kW</p>
          </div>

          <div className="flex flex-col items-center p-4 bg-secondary rounded-lg">
            <Clock className="h-6 w-6 mb-2 text-green-500" />
            <p className="text-xs text-muted-foreground">{t("travelTime")}</p>
            <p className="text-2xl font-bold">{best.travelTime.toFixed(0)}</p>
            <p className="text-xs text-muted-foreground">{t("seconds")}</p>
          </div>
        </div>
      </CardContent>
    </Card>
  );
}
