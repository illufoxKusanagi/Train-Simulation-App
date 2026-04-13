"use client";

import { useState, useEffect, useRef } from "react";
import { api } from "@/services/api";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Form } from "@/components/ui/form";
import { toast } from "sonner";
import { Play, Loader2, Trophy, Activity, Download } from "lucide-react";
import PageLayout from "@/components/page-layout";
import { InputWidget } from "@/components/inputs/input-widget";
import {
  accelerationFormDatas,
  weakeningFormDatas,
  OptimizationFormSchema,
} from "./form.constants";
import { useForm } from "react-hook-form";
import { z } from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { useFormPersistence } from "@/contexts/FormPersistenceContext";
import { useTranslations } from "next-intl";
import { exportTableToCsv } from "@/lib/csv-export";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { WinnerTab } from "./winner-tab";
import { FuzzyMemberTab } from "./fuzzy-member-tab";

interface OptResult {
  acc_start_si: number; // m/s²
  v_p1: number; // km/h
  peakMotorPower: number; // kW/motor
  travelTime: number; // seconds
  fuzzyScore: number; // 0–100
}

function scoreColor(score: number): string {
  if (score >= 75) return "text-green-500";
  if (score >= 50) return "text-yellow-500";
  if (score >= 25) return "text-orange-500";
  return "text-red-500";
}

function scoreLabel(score: number, t: (key: string) => string): string {
  if (score >= 75) return t("excellent");
  if (score >= 50) return t("good");
  if (score >= 25) return t("fair");
  return t("poor");
}

function scoreBadgeClass(score: number): string {
  if (score >= 75)
    return "bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200";
  if (score >= 50)
    return "bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200";
  if (score >= 25)
    return "bg-orange-100 text-orange-800 dark:bg-orange-900 dark:text-orange-200";
  return "bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200";
}

export default function OptimizationPage() {
  const [isRunning, setIsRunning] = useState(false);
  const [isStarting, setIsStarting] = useState(false);
  const [hasStarted, setHasStarted] = useState(false);
  const [results, setResults] = useState<OptResult[]>([]);
  const [best, setBest] = useState<OptResult | null>(null);
  const [completed, setCompleted] = useState(0);
  const [total, setTotal] = useState(20);
  const { saveFormData, loadFormData } = useFormPersistence();
  const t = useTranslations("Optimization");
  const constantForm = useForm<z.infer<typeof OptimizationFormSchema>>({
    resolver: zodResolver(OptimizationFormSchema),
    defaultValues: {
      accelMin: 0.6,
      accelMax: 1.2,
      weakeningMin: 35,
      weakeningMax: 70,
    },
  });

  // Restore persisted fuzzy range values on mount
  useEffect(() => {
    const saved = loadFormData("optimization-params");
    if (saved) {
      constantForm.reset(saved as z.infer<typeof OptimizationFormSchema>);
    }
  }, []); // eslint-disable-line react-hooks/exhaustive-deps

  // Persist whenever the user edits a value
  useEffect(() => {
    const subscription = constantForm.watch((data) => {
      saveFormData("optimization-params", data as Record<string, unknown>);
    });
    return () => subscription.unsubscribe();
  }, [constantForm, saveFormData]);

  const pollingRef = useRef<NodeJS.Timeout | null>(null);

  const stopPolling = () => {
    if (pollingRef.current) {
      clearInterval(pollingRef.current);
      pollingRef.current = null;
    }
  };
  const startPolling = () => {
    stopPolling();
    let inFlight = false;
    pollingRef.current = setInterval(async () => {
      if (inFlight) return;
      inFlight = true;
      try {
        const status = await api.getOptimizationStatus();
        setIsRunning(status.isRunning);
        setResults(status.results);
        setCompleted(status.completedCombinations);
        setTotal(status.totalCombinations);

        const hasBest =
          status.best &&
          typeof status.best === "object" &&
          "fuzzyScore" in status.best;
        setBest(hasBest ? (status.best as OptResult) : null);

        if (!status.isRunning) {
          stopPolling();
          if (status.completedCombinations > 0) {
            toast.success(
              t("toast.complete", {
                count: String(status.completedCombinations),
              }),
            );
          } else {
            toast.error(t("toast.failed"));
            setIsRunning(false);
            setHasStarted(false);
          }
        }
      } catch (error) {
        console.error("Polling error", error);
        stopPolling();
        setIsRunning(false);
        toast.error(t("toast.pollingFailed"));
      } finally {
        inFlight = false;
      }
    }, 1500);
  };

  // On mount: restore previous results from backend (survives page navigation).
  // Backend OptimizationHandler keeps m_results in memory as long as the
  // Qt process is alive, so switching pages and coming back restores state.
  useEffect(() => {
    api
      .getOptimizationStatus()
      .then((status) => {
        if (status.completedCombinations > 0 || status.isRunning) {
          setResults(status.results);
          setCompleted(status.completedCombinations);
          setTotal(status.totalCombinations);
          setHasStarted(true);
          setIsRunning(status.isRunning);
          const hasBest =
            status.best &&
            typeof status.best === "object" &&
            "fuzzyScore" in status.best;
          setBest(hasBest ? (status.best as OptResult) : null);
          if (status.isRunning) startPolling();
        }
      })
      .catch(() => {
        /* backend not ready yet — ignore */
      });
    return () => stopPolling();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  const handleStart = async () => {
    if (isRunning || isStarting) return;
    setIsStarting(true);
    try {
      setResults([]);
      setBest(null);
      setCompleted(0);
      setHasStarted(true);
      const vals = constantForm.getValues();
      const nAcc = Math.round((vals.accelMax - vals.accelMin) / 0.05) + 1;
      const nVp1 = Math.round((vals.weakeningMax - vals.weakeningMin) / 5) + 1;
      await api.startOptimization(vals);
      toast.success(t("toast.started", { combinations: String(nAcc * nVp1) }));
      setIsRunning(true);
      startPolling();
    } catch (error) {
      console.error(error);
      toast.error(
        error instanceof Error ? error.message : t("toast.startFailed"),
      );
      setIsRunning(false);
      setHasStarted(false);
    } finally {
      setIsStarting(false);
    }
  };

  const progressPct = total > 0 ? Math.round((completed / total) * 100) : 0;

  return (
    <PageLayout>
      <div className="flex flex-col w-full h-full p-6 gap-4">
        {/* Header */}
        <div className="flex justify-between w-full">
          <div className="w-full">
            <div className="flex flex-row justify-between w-full">
              <div className="flex flex-col">
                <p className="heading-2 tracking-tight">{t("title")}</p>
                <p className="text-muted-foreground mt-1">{t("description")}</p>
              </div>
              <div className="flex gap-2">
                <Button
                  onClick={handleStart}
                  disabled={isRunning || isStarting}
                  className="bg-primary disabled:opacity-60"
                >
                  {isRunning ? (
                    <>
                      <Loader2 className="mr-2 h-4 w-4 animate-spin" />{" "}
                      {t("running")}
                    </>
                  ) : (
                    <>
                      <Play className="mr-2 h-4 w-4" /> {t("startOptimization")}
                    </>
                  )}
                </Button>
                <Button
                  variant="outline"
                  disabled={results.length === 0}
                  onClick={() =>
                    exportTableToCsv(
                      results,
                      [
                        { key: "acc_start_si", header: "acc_start_si (m/s²)" },
                        { key: "v_p1", header: "v_p1 (km/h)" },
                        {
                          key: "peakMotorPower",
                          header: "Peak Power/Motor (kW)",
                        },
                        { key: "travelTime", header: "Travel Time (s)" },
                        { key: "fuzzyScore", header: "Fuzzy Score" },
                      ],
                      "optimization-results.csv",
                      t("toast.exportSuccess"),
                    )
                  }
                >
                  <Download className="mr-2 h-4 w-4" /> {t("saveResultsCsv")}
                </Button>
              </div>
            </div>
            <div className="w-full">
              <Card className="w-full">
                <CardHeader>
                  <p className="heading-3">{t("searchSpaceTitle")}</p>
                  <p className="text-muted-foreground">
                    {t("searchSpaceDescription")}
                  </p>
                </CardHeader>
                <CardContent className="space-y-6">
                  <Form {...constantForm}>
                    {/* Acceleration */}
                    <div className="space-y-2">
                      <p className="text-sm font-semibold">
                        {t("acceleration")}
                      </p>
                      <div className="grid grid-cols-2 gap-4">
                        {accelerationFormDatas.map((formData) => (
                          <InputWidget
                            key={formData.name}
                            inputType={formData}
                            control={constantForm.control}
                          />
                        ))}
                      </div>
                    </div>
                    {/* Weakening */}
                    <div className="space-y-2">
                      <p className="text-sm font-semibold">
                        {t("weakeningPoint")}
                      </p>
                      <div className="grid grid-cols-2 gap-4">
                        {weakeningFormDatas.map((formData) => (
                          <InputWidget
                            key={formData.name}
                            inputType={formData}
                            control={constantForm.control}
                          />
                        ))}
                      </div>
                    </div>
                  </Form>
                </CardContent>
              </Card>
            </div>
          </div>
        </div>

        {/* Progress bar */}
        {hasStarted && (
          <div className="space-y-1">
            <div className="flex justify-between text-sm text-muted-foreground">
              <span>{t("progress")}</span>
              <span>
                {completed} / {total} {t("combinations")}
              </span>
            </div>
            <div
              className="w-full bg-secondary rounded-full h-2"
              role="progressbar"
              aria-valuemin={0}
              aria-valuemax={total}
              aria-valuenow={completed}
              aria-label="Optimization progress"
            >
              <div
                className="bg-primary h-2 rounded-full transition-all duration-500"
                style={{ width: `${progressPct}%` }}
              />
            </div>
          </div>
        )}

        {/* Optimization Results Tabs */}
        {best && results.length > 0 && (
          <Tabs defaultValue="winners" className="w-full mt-4">
            <TabsList className="grid w-full grid-cols-2">
              <TabsTrigger value="winners">
                {t("bestCombinationTab")}
              </TabsTrigger>
              <TabsTrigger value="fuzzy-membership-chart">
                {t("fuzzyMembershipTab")}
              </TabsTrigger>
            </TabsList>
            <TabsContent value="winners">
              <WinnerTab best={best} t={t} />
            </TabsContent>
            <TabsContent value="fuzzy-membership-chart">
              <FuzzyMemberTab results={results} best={best} />
            </TabsContent>
          </Tabs>
        )}

        {/* Results Table */}
        {results.length > 0 && (
          <Card>
            <CardHeader>
              <CardTitle>
                {t("allCombinations")} ({results.length} / {total})
              </CardTitle>
            </CardHeader>
            <CardContent className="overflow-x-auto">
              <table className="w-full text-sm">
                <thead>
                  <tr className="border-b text-muted-foreground">
                    <th className="text-left py-2 pr-4 font-semibold">#</th>
                    <th className="text-right py-2 pr-4 font-semibold">
                      acc_start_si (m/s²)
                    </th>
                    <th className="text-right py-2 pr-4 font-semibold">
                      v_p1 (km/h)
                    </th>
                    <th className="text-right py-2 pr-4 font-semibold">
                      Peak Power/Motor (kW)
                    </th>
                    <th className="text-right py-2 pr-4 font-semibold">
                      Travel Time (s)
                    </th>
                    <th className="text-right py-2 font-semibold">
                      {t("fuzzyScore")}
                    </th>
                    <th className="text-center py-2 pl-4 font-semibold">
                      {t("grade")}
                    </th>
                  </tr>
                </thead>
                <tbody>
                  {/* {results.map((r, i) => {
                    const isBest =
                      best &&
                      r.acc_start_si === best.acc_start_si &&
                      r.v_p1 === best.v_p1; */}
                  {results.map((r, i) => {
                    const sameOptResult = (a: OptResult, b: OptResult) =>
                      Math.abs(a.acc_start_si - b.acc_start_si) < 1e-6 &&
                      Math.abs(a.v_p1 - b.v_p1) < 1e-6;

                    const isBest = best && sameOptResult(r, best);
                    return (
                      <tr
                        key={i}
                        className={`border-b transition-colors ${
                          isBest
                            ? "bg-yellow-50 dark:bg-yellow-950 font-semibold"
                            : "hover:bg-muted/30"
                        }`}
                      >
                        <td className="py-2 pr-4">
                          {isBest ? (
                            <span className="flex items-center gap-1">
                              <Trophy className="h-3 w-3 text-yellow-500" />
                              {i + 1}
                            </span>
                          ) : (
                            i + 1
                          )}
                        </td>
                        <td className="text-right py-2 pr-4 font-mono">
                          {r.acc_start_si.toFixed(2)}
                        </td>
                        <td className="text-right py-2 pr-4 font-mono">
                          {r.v_p1.toFixed(1)}
                        </td>
                        <td className="text-right py-2 pr-4 font-mono">
                          {r.peakMotorPower.toFixed(1)}
                        </td>
                        <td className="text-right py-2 pr-4 font-mono">
                          {r.travelTime.toFixed(0)}
                        </td>
                        <td
                          className={`text-right py-2 pr-2 font-mono font-bold ${scoreColor(
                            r.fuzzyScore,
                          )}`}
                        >
                          {r.fuzzyScore.toFixed(2)}
                        </td>
                        <td className="text-center py-2 pl-4">
                          <span
                            className={`text-xs font-semibold px-2 py-0.5 rounded-full ${scoreBadgeClass(
                              r.fuzzyScore,
                            )}`}
                          >
                            {scoreLabel(r.fuzzyScore, t)}
                          </span>
                        </td>
                      </tr>
                    );
                  })}
                </tbody>
              </table>
            </CardContent>
          </Card>
        )}

        {/* Empty state */}
        {!hasStarted && (
          <Card className="border-dashed">
            <CardContent className="flex flex-col items-center justify-center py-16 gap-4 text-muted-foreground">
              <Activity className="h-12 w-12 opacity-30" />
              <p className="text-lg font-medium">{t("noResults")}</p>
              <p className="text-sm">{t("noResultsDescription")}</p>
            </CardContent>
          </Card>
        )}
      </div>
    </PageLayout>
  );
}
