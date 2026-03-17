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
import { useForm } from "react-hook-form";
import { constantFormRows, RunningFormSchema } from "./form.constants";
import { useState, useEffect } from "react";
import z from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { toast } from "sonner";
import { Form } from "@/components/ui/form";
import { api } from "@/services/api";
import { initializeBackendOnce } from "@/lib/backendInit";
import { Spinner } from "@/components/ui/spinner";
import { isQtWebChannelReady, openFileWithDialog } from "@/lib/qt-webchannel";
import { useRef } from "react";
import { useFormPersistence } from "@/contexts/FormPersistenceContext";

export default function RunningPage() {
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [isUploading, setIsUploading] = useState(false);
  const csvInputRef = useRef<HTMLInputElement>(null);
  const { saveFormData, loadFormData, clearFormData } = useFormPersistence();

  const defaultValues = {
    startRes: 39.2,
    v_diffCoast: 5,
    acc_start: 0.8,
    v_p1: 35,
    v_p2: 65,
    decc_start: 1,
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
        toast.error("Could not load saved parameters — using defaults");
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
      //   acc_start: data.acc_start,
      //   v_p1: data.v_p1,
      //   v_p2: data.v_p2,
      //   v_b1: data.v_b1,
      //   v_b2: data.v_b2,
      //   decc_start: data.decc_start,
      //   decc_emergency: data.decc_emergency,
      // };

      const result = await api.updateRunningParameters(data);
      console.log("Backend response:", result);
      toast.success("Success!", {
        description: "Running parameters updated successfully",
      });
    } catch (error) {
      console.error("Error updating parameters:", error);
      toast.error("Error!", {
        description: "Failed to save data. Please try again.",
      });
    } finally {
      setIsSubmitting(false);
    }
  }

  /**
   * Handle CSV File Upload
   *
   * This function parses a CSV file and populates the form fields.
   *
   * Expected CSV Format (Simple Key-Value):
   * ---------------------------------------
   * startRes,39.2
   * v_diffCoast,5
   * acc_start,1
   * ...
   *
   * Logic:
   * 1. Reads file as text.
   * 2. Splits content by newlines to get rows.
   * 3. Splits each row by comma to get [key, value].
   * 4. Checks if the key exists in our form schema.
   * 5. Validates that the value is a number.
   * 6. Updates the form field using setValue.
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

    lines.forEach((line) => {
      if (!line.trim()) return;
      const [key, valueStr] = line.split(/,(.+)/);
      const cleanKey = key?.trim();
      const cleanValue = valueStr?.trim();
      if (!cleanKey || !cleanValue) return;

      const validKeys: (keyof z.infer<typeof RunningFormSchema>)[] = [
        "startRes",
        "v_diffCoast",
        "acc_start",
        "v_p1",
        "v_p2",
        "decc_start",
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
    toast("Form berhasil direset!");
  };

  return (
    <PageLayout>
      <Card className="px-6 py-8 min-h-[40rem] h-fit w-full max-w-2xl rounded-3xl justify-center">
        <CardHeader className="text-center">
          <CardTitle className="text-2xl">Running Parameter</CardTitle>
          <CardDescription>
            Input related to Running configuration
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

              <div className="flex gap-4 pt-4">
                <Button
                  type="submit"
                  className="flex-1"
                  disabled={isSubmitting}
                >
                  {isSubmitting ? (
                    <>
                      <Spinner className="mr-2" />
                      Saving...
                    </>
                  ) : (
                    "Save"
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
                          "Select Running Parameters CSV File",
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
                        Uploading...
                      </>
                    ) : (
                      "Upload CSV"
                    )}
                  </Button>
                </div>
                <Button
                  type="button"
                  variant="outline"
                  className="flex-1"
                  onClick={handleReset}
                >
                  Reset
                </Button>
              </div>
            </form>
          </Form>
        </CardContent>
      </Card>
    </PageLayout>
  );
}
