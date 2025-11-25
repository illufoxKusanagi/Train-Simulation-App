"use client";

import { useState } from "react";
import { Button } from "@/components/ui/button";
import { Play, Loader2 } from "lucide-react";
import { api } from "@/services/api";
import { toast } from "@/lib/toast";
import { useRouter } from "next/navigation";

export function SimulationButtons() {
  const [isRunningStatic, setIsRunningStatic] = useState(false);
  const [isRunningDynamic, setIsRunningDynamic] = useState(false);
  const router = useRouter();

  const runSimulation = async (type: "static" | "dynamic") => {
    const setLoading =
      type === "static" ? setIsRunningStatic : setIsRunningDynamic;

    setLoading(true);
    try {
      // REMOVED: quickInit() was resetting all parameters to default/zero values!
      // Users must submit parameters from the parameter pages first
      // quickInit should only be called once at app startup, not before every simulation

      // Start simulation with current parameters - this returns the summary
      await api.startSimulation({ type });

      // Poll status until complete
      let statusResult = await api.getSimulationStatus();
      while (statusResult.isRunning) {
        await new Promise((resolve) => setTimeout(resolve, 500));
        statusResult = await api.getSimulationStatus();
      }

      // Fetch detailed results
      const resultsResponse = await api.getSimulationResults();

      // Combine summary from status with results from getResults
      const response = {
        ...resultsResponse,
        summary: statusResult.summary, // Use summary from status endpoint
      };

      // Store combined results in sessionStorage for output page
      sessionStorage.setItem("simulationResults", JSON.stringify(response));

      // Safely get maxSpeed with fallback
      const maxSpeed = statusResult.summary?.maxSpeed ?? 0;

      toast.success(
        `${type === "static" ? "Static" : "Dynamic"} simulation completed!`,
        `Max speed: ${maxSpeed.toFixed(2)} km/h`
      );

      // Navigate to output page
      router.push("/output");
    } catch (error) {
      console.error("Simulation failed:", error);
      toast.error(
        "Simulation failed!",
        "Make sure all parameters are set correctly."
      );
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
            Running...
          </>
        ) : (
          <>
            <Play className="h-4 w-4 mr-2" />
            Static
          </>
        )}
      </Button>
      <Button
        onClick={() => runSimulation("dynamic")}
        disabled={isRunningStatic || isRunningDynamic}
        variant="outline"
        size="sm"
      >
        {isRunningDynamic ? (
          <>
            <Loader2 className="h-4 w-4 animate-spin mr-2" />
            Running...
          </>
        ) : (
          <>
            <Play className="h-4 w-4 mr-2" />
            Dynamic
          </>
        )}
      </Button>
    </div>
  );
}
