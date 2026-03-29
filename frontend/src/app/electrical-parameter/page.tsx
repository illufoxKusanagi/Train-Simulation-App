"use client";

import { InputWidget } from "@/components/inputs/input-widget";
import PageLayout from "@/components/page-layout";
import { Button } from "@/components/ui/button";
import {
  Card,
  CardHeader,
  CardTitle,
  CardDescription,
  CardContent,
} from "@/components/ui/card";
import { zodResolver } from "@hookform/resolvers/zod";
import { useState, useEffect } from "react";
import { useForm } from "react-hook-form";
import { toast } from "sonner";
import z from "zod";
import { constantFormRows, ElectricalFormSchema } from "./form.constants";
import { Form } from "@/components/ui/form";
import { api } from "@/services/api";
import { initializeBackendOnce } from "@/lib/backendInit";
import { Spinner } from "@/components/ui/spinner";
import { isQtWebChannelReady, openFileWithDialog } from "@/lib/qt-webchannel";
import { useRef } from "react";
import { useFormPersistence } from "@/contexts/FormPersistenceContext";
import { useTranslations } from "next-intl";

export default function ElectricalParameterPage() {
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [isUploading, setIsUploading] = useState(false);
  const [csvData, setCsvData] = useState<Record<string, number[][]>>({});
  const csvInputRef = useRef<HTMLInputElement>(null);
  const { saveFormData, loadFormData, clearFormData } = useFormPersistence();

  const trans = useTranslations("ElectricalParams");

  const defaultValues = {
    stat_vol_line: 1500,
    stat_vol_motor: 1200,
    stat_pf: 0,
    stat_eff_gear: 98,
    stat_eff_motor: 89,
    stat_eff_vvvf: 96,
    p_aps: 30,
  };

  const constantForm = useForm<z.infer<typeof ElectricalFormSchema>>({
    resolver: zodResolver(ElectricalFormSchema),
    defaultValues,
  });

  useEffect(() => {
    const savedData = loadFormData("electrical-params");
    const hasSavedData = savedData && Object.keys(savedData).length > 0;
    const loadDefaults = async () => {
      try {
        await initializeBackendOnce();
        if (hasSavedData) {
          constantForm.reset({
            ...defaultValues,
            ...(savedData as z.infer<typeof ElectricalFormSchema>),
          });
          return;
        }
        const data = await api.getElectricalParameters();
        constantForm.reset({
          ...defaultValues,
          ...data.electricalParameters,
        });
      } catch (err) {
        console.error("Failed to load electrical parameters:", err);
        toast.error(trans("uploadCsvFailed"));
        constantForm.reset(defaultValues);
      }
    };
    loadDefaults();
  }, []); // eslint-disable-line react-hooks/exhaustive-deps

  useEffect(() => {
    const subscription = constantForm.watch((data) => {
      saveFormData("electrical-params", data as Record<string, unknown>);
    });
    return () => subscription.unsubscribe();
  }, [constantForm, saveFormData]);

  const handleFileLoad = (name: string, data: number[][]) => {
    setCsvData((prev) => ({
      ...prev,
      [name]: data,
    }));
    console.log(`File loaded for ${name}:`, data);
  };

  async function onSubmit(data: z.infer<typeof ElectricalFormSchema>) {
    setIsSubmitting(true);
    try {
      console.log("Form Data:", data);
      console.log("CSV Data:", csvData);

      // Send using YOUR exact variable names - NO CONVERSION
      const electricalParams = {
        stat_vol_line: data.stat_vol_line,
        stat_vol_motor: data.stat_vol_motor,
        stat_pf: data.stat_pf,
        stat_eff_gear: data.stat_eff_gear,
        stat_eff_motor: data.stat_eff_motor,
        stat_eff_vvvf: data.stat_eff_vvvf,
        p_aps: data.p_aps,
      };

      const result = await api.updateElectricalParameters(electricalParams);
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

  const handleReset = () => {
    constantForm.reset(defaultValues);
    clearFormData("electrical-params");
    setCsvData({});
    toast(trans("toast.reset"));
  };

  /**
   * Handle CSV File Upload (Batch Config)
   *
   * Expected CSV Format (Simple Key-Value):
   * ---------------------------------------
   * stat_vol_line,1500
   * stat_eff_motor,89
   * ...
   */
  const handleCsvUpload = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();

    reader.onload = (e) => {
      const text = e.target?.result as string;
      if (!text) return;
      processCsvText(text);
      event.target.value = "";
    };
    reader.readAsText(file);
  };

  const processCsvText = (text: string) => {
    console.log("📂 processing CSV upload...");
    const lines = text.split(/\r\n|\n/);
    let successCount = 0;
    let errorCount = 0;

    const validKeys = Object.keys(ElectricalFormSchema.shape);

    lines.forEach((line) => {
      if (!line.trim()) return;
      const [key, valueStr] = line.split(/,(.+)/);
      const cleanKey = key?.trim();
      const cleanValue = valueStr?.trim();

      if (!cleanKey || !cleanValue) return;

      if (validKeys.includes(cleanKey) && !isNaN(Number(cleanValue))) {
        constantForm.setValue(
          cleanKey as keyof z.infer<typeof ElectricalFormSchema>,
          Number(cleanValue),
          {
            shouldDirty: true,
            shouldValidate: true,
          },
        );
        successCount++;
        console.log(`✅ Set ${cleanKey} = ${cleanValue}`);
      } else {
        console.warn(`⚠️ Skipped invalid item: ${cleanKey}`);
        errorCount++;
      }
    });

    if (successCount > 0) toast.success(`Updated ${successCount} fields`);
    if (errorCount > 0) toast.warning(`Skipped ${errorCount} invalid items`);
  };

  return (
    <PageLayout>
      <Card className="px-6 py-8 min-h-[40rem] h-fit w-full max-w-2xl rounded-3xl justify-center">
        <CardHeader className="text-center">
          <CardTitle className="text-2xl">{trans("title")}</CardTitle>
          <CardDescription>
            {trans("description")}
          </CardDescription>
        </CardHeader>
        <CardContent>
          <Form {...constantForm}>
            <form
              onSubmit={constantForm.handleSubmit(onSubmit)}
              className="space-y-6"
            >
              <div className="flex flex-col gap-6">
                {constantFormRows.map((row, rowIndex) => (
                  <div key={rowIndex} className="grid grid-cols-2 gap-4">
                    {row.map((inputType) => (
                      <InputWidget
                        key={inputType.name}
                        inputType={inputType}
                        control={constantForm.control}
                        onFileLoad={handleFileLoad}
                      />
                    ))}
                    {row.length < 2 &&
                      Array.from({ length: 2 - row.length }).map(
                        (_, emptyIndex) => (
                          <div key={`empty-${rowIndex}-${emptyIndex}`} />
                        ),
                      )}
                  </div>
                ))}
              </div>

              <div className="flex gap-4 pt-4">
                <Button
                  type="submit"
                  className="flex-1"
                  disabled={isSubmitting}
                >
                  {isSubmitting ? (
                    <>
                      <Spinner className="mr-2" />
                      {trans("saving")}
                    </>
                  ) : (
                    trans("save")
                  )}
                </Button>
                <div className="flex-1 relative">
                  <input
                    ref={csvInputRef}
                    type="file"
                    accept=".csv"
                    className="hidden"
                    onChange={handleCsvUpload}
                  />
                  <Button
                    type="button"
                    variant="default"
                    className="w-full"
                    disabled={isUploading}
                    onClick={async () => {
                      if (isQtWebChannelReady()) {
                        setIsUploading(true);
                        const result = await openFileWithDialog(
                          "Select Electrical Parameters CSV File",
                          "CSV Files (*.csv);;All Files (*)",
                        );
                        if (result.success && result.content)
                          processCsvText(result.content);
                        setIsUploading(false);
                      } else {
                        csvInputRef.current?.click();
                      }
                    }}
                  >
                    {isUploading ? (
                      <>
                        <Spinner className="mr-2" />
                        {trans("uploading")}
                      </>
                    ) : (
                      trans("uploadCsv")
                    )}
                  </Button>
                </div>
                <Button
                  type="button"
                  variant="outline"
                  className="flex-1"
                  onClick={handleReset}
                >
                  {trans("reset")}
                </Button>
              </div>
            </form>
          </Form>
        </CardContent>
      </Card>
    </PageLayout>
  );
}
