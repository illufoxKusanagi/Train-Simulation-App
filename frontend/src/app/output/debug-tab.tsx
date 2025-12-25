"use client";

import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { ScrollArea } from "@/components/ui/scroll-area";
import type { SimulationResults } from "@/services/api";

interface DebugTabProps {
  results: SimulationResults;
}

export default function DebugTab({ results }: DebugTabProps) {
  const logs = results.debugInfo?.logs || [];

  return (
    <div className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>Simulation Trace Logs</CardTitle>
          <CardDescription>
            Detailed internal state of the simulation (first 20 iterations)
          </CardDescription>
        </CardHeader>
        <CardContent>
          <ScrollArea className="h-[400px] w-full rounded-md border p-4 bg-muted/50 font-mono text-xs">
            {logs.length > 0 ? (
              logs.map((log, index) => (
                <div key={index} className="mb-1 whitespace-pre-wrap break-all">
                  {log}
                </div>
              ))
            ) : (
              <div className="text-muted-foreground italic">
                No debug logs available.
              </div>
            )}
          </ScrollArea>
        </CardContent>
      </Card>
    </div>
  );
}
