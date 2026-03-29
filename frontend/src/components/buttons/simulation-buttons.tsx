"use client";

import { useState } from "react";
import { Button } from "@/components/ui/button";
import { Play, Loader2 } from "lucide-react";
import { api } from "@/services/api";
import { toast } from "@/lib/toast";
import { useRouter } from "next/navigation";
import { useTranslations } from "next-intl";

/**
 * Translate a backend warning/error key like "WARN_CSV_EMPTY:Motor efficiency"
 * into the user's locale.  Keys may contain a `:` separator whose RHS is a
 * dynamic parameter (dataName).
 */
function translateWarning(
  key: string,
  tw: (k: string, params?: Record<string, string>) => string,
): string {
  const [base, param] = key.split(":");
  try {
    if (param) {
      return tw(base, { dataName: param });
    }
    return tw(base);
  } catch {
    // Fallback: return raw key if translation is missing
    return key;
  }
}

export function SimulationButtons() {
  const [isRunningStatic, setIsRunningStatic] = useState(false);
  const [isRunningDynamic, setIsRunningDynamic] = useState(false);
  const router = useRouter();
  const t = useTranslations("Simulation");
  const tw = useTranslations("warnings");
  const te = useTranslations("errors");

  const runSimulation = async (type: "static" | "dynamic") => {
    const setLoading =
      type === "static" ? setIsRunningStatic : setIsRunningDynamic;

    setLoading(true);
    try {
      await api.startSimulation({ type });

      let statusResult = await api.getSimulationStatus();
      while (statusResult.isRunning) {
        await new Promise((resolve) => setTimeout(resolve, 500));
        statusResult = await api.getSimulationStatus();
      }

      window.dispatchEvent(new Event("simulationUpdated"));

      const maxSpeed = statusResult.summary?.maxSpeed ?? 0;

      toast.success(
        t("completed", { type: type === "static" ? t("static") : t("dynamic") }),
        { description: t("maxSpeed", { speed: maxSpeed.toFixed(2) }) },
      );

      // Display warnings — translate the backend keys
      if (statusResult.warnings && statusResult.warnings.length > 0) {
        statusResult.warnings.forEach((warning) => {
          toast.warning(t("warning"), {
            description: translateWarning(warning, tw),
            duration: 5000,
          });
        });
      }

      // Display errors — translate the backend keys
      if (statusResult.errors && statusResult.errors.length > 0) {
        statusResult.errors.forEach((error) => {
          toast.error(t("error"), {
            description: translateWarning(error, te),
            duration: 8000,
          });
        });
      }

      router.push("/output");
    } catch (error) {
      console.error("Simulation failed:", error);
      toast.error(t("failed"), t("failedDescription"));
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="flex gap-2">
      <Button
        onClick={() => runSimulation("static")}
        disabled={isRunningStatic || isRunningDynamic}
        variant="default"
        size="sm"
      >
        {isRunningStatic ? (
          <>
            <Loader2 className="h-4 w-4 animate-spin mr-2" />
            {t("running")}
          </>
        ) : (
          <>
            <Play className="h-4 w-4 mr-2" />
            {t("static")}
          </>
        )}
      </Button>
      <Button
        onClick={() => runSimulation("dynamic")}
        disabled={isRunningStatic || isRunningDynamic}
        variant="default"
        size="sm"
      >
        {isRunningDynamic ? (
          <>
            <Loader2 className="h-4 w-4 animate-spin mr-2" />
            {t("running")}
          </>
        ) : (
          <>
            <Play className="h-4 w-4 mr-2" />
            {t("dynamic")}
          </>
        )}
      </Button>
    </div>
  );
}
