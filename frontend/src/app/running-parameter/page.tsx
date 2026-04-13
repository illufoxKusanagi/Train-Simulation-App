"use client";

import { InputWidget } from "@/components/inputs/input-widget";
import PageLayout from "@/components/page-layout";
import {
  Card,
  CardHeader,
  CardTitle,
  CardDescription,
  CardContent,
} from "@/components/ui/card";
import { useForm } from "react-hook-form";
import { constantFormRows, RunningFormSchema } from "./form.constants";
import { useState, useEffect } from "react";
import z from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { toast } from "sonner";
import { Form } from "@/components/ui/form";
import { api } from "@/services/api";
import { initializeBackendOnce } from "@/lib/backendInit";
import { useFormPersistence } from "@/contexts/FormPersistenceContext";
import { useTranslations } from "next-intl";
import { exportConfigToCsv } from "@/lib/csv-export";
import { FormActionButtons } from "@/components/buttons/form-action-buttons";

export default function RunningPage() {
  const trans = useTranslations("RunningParams");
  const [isSubmitting, setIsSubmitting] = useState(false);
  const { saveFormData, loadFormData, clearFormData } = useFormPersistence();

  const defaultValues = {
    startRes: 39.2,
    v_diffCoast: 5,
    acc_start_si: 0.8,
    v_p1: 35,
    v_p2: 65,
    decc_start_si: 1,
    decc_emergency: 1.2,
    v_b1: 55,
    v_b2: 70,
  };

  const constantForm = useForm<z.infer<typeof RunningFormSchema>>({
    resolver: zodResolver(RunningFormSchema),
    defaultValues,
  });

  useEffect(() => {
    const savedData = loadFormData("running-params");
    const hasSavedData = savedData && Object.keys(savedData).length > 0;

    if (hasSavedData) {
      constantForm.reset({
        ...defaultValues,
        ...(savedData as z.infer<typeof RunningFormSchema>),
      });
      return;
    }

    const loadDefaults = async () => {
      try {
        await initializeBackendOnce();
        const data = await api.getRunningParameters();
        constantForm.reset({
          ...defaultValues,
          ...data.runningParameters,
        });
      } catch (err) {
        console.error("Failed to load running parameters:", err);
        toast.error(trans("uploadCsvFailed"));
        constantForm.reset(defaultValues);
      }
    };

    loadDefaults();
  }, []); // eslint-disable-line react-hooks/exhaustive-deps

  useEffect(() => {
    const subscription = constantForm.watch((data) => {
      saveFormData("running-params", data as Record<string, unknown>);
    });
    return () => subscription.unsubscribe();
  }, [constantForm, saveFormData]);

  async function onSubmit(data: z.infer<typeof RunningFormSchema>) {
    setIsSubmitting(true);
    try {
      console.log("Form Data:", data);

      // Use YOUR exact field names from RunningParams - NO CONVERSION
      // const params = {
      //   startRes: data.startRes,
      //   v_diffCoast: data.v_diffCoast,
      //   acc_start_si: data.acc_start_si,
      //   v_p1: data.v_p1,
      //   v_p2: data.v_p2,
      //   v_b1: data.v_b1,
      //   v_b2: data.v_b2,
      //   decc_start_si: data.decc_start_si,
      //   decc_emergency: data.decc_emergency,
      // };

      const result = await api.updateRunningParameters(data);
      console.log("Backend response:", result);
      toast.success(trans("toast.success"), {
        description: trans("toast.successDescription"),
      });
    } catch (error) {
      console.error("Error updating parameters:", error);
      toast.error(trans("toast.error"), {
        description: trans("toast.errorDescription"),
      });
    } finally {
      setIsSubmitting(false);
    }
  }

  const processCsvText = (text: string) => {
    console.log("📂 processing CSV upload...");
    const lines = text.split(/\r\n|\n/);
    let successCount = 0;
    let errorCount = 0;

    lines.forEach((line) => {
      if (!line.trim()) return;
      const [key, valueStr] = line.split(/,(.+)/);
      const cleanKey = key?.trim();
      const cleanValue = valueStr?.trim();
      if (!cleanKey || !cleanValue) return;

      const validKeys: (keyof z.infer<typeof RunningFormSchema>)[] = [
        "startRes",
        "v_diffCoast",
        "acc_start_si",
        "v_p1",
        "v_p2",
        "decc_start_si",
        "decc_emergency",
        "v_b1",
        "v_b2",
      ];

      if (
        validKeys.includes(
          cleanKey as keyof z.infer<typeof RunningFormSchema>,
        ) &&
        !isNaN(Number(cleanValue))
      ) {
        constantForm.setValue(
          cleanKey as keyof z.infer<typeof RunningFormSchema>,
          Number(cleanValue),
          { shouldDirty: true, shouldValidate: true },
        );
        successCount++;
      } else {
        errorCount++;
      }
    });

    if (successCount > 0)
      toast.success(`Updated ${successCount} fields from CSV`);
    if (errorCount > 0) toast.warning(`Skipped ${errorCount} invalid lines`);
  };

  const handleReset = () => {
    constantForm.reset(defaultValues);
    clearFormData("running-params");
    toast(trans("toast.reset"));
  };

  return (
    <PageLayout>
      <Card className="px-6 py-8 min-h-[40rem] h-fit w-full max-w-2xl rounded-3xl justify-center">
        <CardHeader className="text-center">
          <CardTitle className="text-2xl">{trans("title")}</CardTitle>
          <CardDescription>{trans("description")}</CardDescription>
        </CardHeader>
        <CardContent>
          <Form {...constantForm}>
            <form
              onSubmit={constantForm.handleSubmit(onSubmit)}
              className="space-y-6"
            >
              <div className="flex flex-col gap-6">
                {constantFormRows.map((row, rowIndex) => (
                  <div key={rowIndex} className="grid grid-cols-3 gap-4">
                    {row.map((inputType) => (
                      <InputWidget
                        key={inputType.name}
                        inputType={inputType}
                        control={constantForm.control}
                      />
                    ))}
                    {row.length < 3 &&
                      Array.from({ length: 3 - row.length }).map(
                        (_, emptyIndex) => (
                          <div key={`empty-${rowIndex}-${emptyIndex}`} />
                        ),
                      )}
                  </div>
                ))}
              </div>

              <FormActionButtons
                isSubmitting={isSubmitting}
                onProcessCsvText={processCsvText}
                onReset={handleReset}
                onExport={() =>
                  exportConfigToCsv(
                    constantForm.getValues(),
                    "running-parameters.csv",
                    trans("exportSuccess"),
                  )
                }
                dialogTitle="Select Running Parameters CSV File"
                labels={{
                  save: trans("save"),
                  saving: trans("saving"),
                  uploadCsv: trans("uploadCsv"),
                  uploading: trans("uploading"),
                  reset: trans("reset"),
                  exportCsv: trans("exportCsv"),
                }}
              />
            </form>
          </Form>
        </CardContent>
      </Card>
    </PageLayout>
  );
}
