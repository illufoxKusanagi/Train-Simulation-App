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
import { useTranslations } from "next-intl";

interface DebugTabProps {
  results: SimulationResults;
}

export default function DebugTab({ results }: DebugTabProps) {
  const t = useTranslations("Outputs");
  const logs = results.debugInfo?.logs || [];

  return (
    <div className="space-y-4">
      <Card>
        <CardHeader>
          <CardTitle>{t("charts.debug.title")}</CardTitle>
          <CardDescription>
            {t("charts.debug.description")}
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
                {t("charts.debug.noLogs")}
              </div>
            )}
          </ScrollArea>
        </CardContent>
      </Card>
    </div>
  );
}
