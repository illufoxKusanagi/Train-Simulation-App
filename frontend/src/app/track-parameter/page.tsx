"use client";

import z from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import {
  constantFormRows,
  slopeFormRows,
  TrackFormSchema,
} from "./form.constants";
import { useState, useEffect } from "react";
import { toast } from "sonner";
import { api } from "@/services/api";
import { initializeBackendOnce } from "@/lib/backendInit";
import { Spinner } from "@/components/ui/spinner";
import { isQtWebChannelReady, openFileWithDialog } from "@/lib/qt-webchannel";
import { useRef } from "react";
import { useFormPersistence } from "@/contexts/FormPersistenceContext";
import PageLayout from "@/components/page-layout";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { InputWidget } from "@/components/inputs/input-widget";
import { Button } from "@/components/ui/button";
import { Form } from "@/components/ui/form";
import { useForm } from "react-hook-form";
import { useTranslations } from "next-intl";

export default function TrackParameterPage() {
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [isUploading, setIsUploading] = useState(false);
  const [csvData, setCsvData] = useState<Record<string, number[][]>>({});
  const csvInputRef = useRef<HTMLInputElement>(null);
  const { saveFormData, loadFormData, clearFormData } = useFormPersistence();
  const trans = useTranslations("TrackParams");

  const defaultValues = {
    n_station: 2,
    x_station: 2000,
    radius: 2000,
    slope: 0,
    v_limit: 80,
    dwellTime: 60,
    slope_option1: 0,
    slope_option2: 5,
    slope_option3: 10,
    slope_option4: 25,
  };

  const constantForm = useForm<z.infer<typeof TrackFormSchema>>({
    resolver: zodResolver(TrackFormSchema),
    defaultValues,
  });

  useEffect(() => {
    const savedData = loadFormData("track-params");
    const hasSavedData = savedData && Object.keys(savedData).length > 0;
    const loadDefaults = async () => {
      try {
        await initializeBackendOnce();
        if (hasSavedData) {
          constantForm.reset({
            ...defaultValues,
            ...(savedData as z.infer<typeof TrackFormSchema>),
          });
          return;
        }
        const data = await api.getTrackParameters();
        constantForm.reset({
          ...defaultValues,
          ...data.trackParameters,
        });
      } catch (err) {
        console.error("Failed to load track parameters:", err);
        toast.error(trans("uploadCsvFailed"));
        constantForm.reset(defaultValues);
      }
    };
    loadDefaults();
  }, []); // eslint-disable-line react-hooks/exhaustive-deps

  // useEffect(() => {
  //   const savedData = loadFormData("track-params");
  //   const hasSavedData = savedData && Object.keys(savedData).length > 0;

  //   if (hasSavedData) {
  //     constantForm.reset({
  //       ...defaultValues,
  //       ...(savedData as z.infer<typeof TrackFormSchema>),
  //     });
  //   } else {
  //     const loadDefaults = async () => {
  //       try {
  //         await initializeBackendOnce();
  //         const data = await api.getTrackParameters();
  //         constantForm.reset({
  //           ...defaultValues,
  //           ...(data.trackParameters as z.infer<typeof TrackFormSchema>),
  //         });
  //       } catch (err) {
  //         console.error("Failed to load track parameters:", err);
  //         toast.error("Could not load saved parameters — using defaults");
  //         constantForm.reset(defaultValues);
  //       }
  //     };

  //     loadDefaults();
  //   }

  //   // Restore uploaded CSV array data
  //   const savedCsvData = localStorage.getItem("track-csv-data");
  //   if (savedCsvData) {
  //     try {
  //       setCsvData(JSON.parse(savedCsvData));
  //     } catch (e) {
  //       console.error("Failed to restore track CSV data:", e);
  //     }
  //   }
  // }, []); // eslint-disable-line react-hooks/exhaustive-deps

  // Persist csvData whenever it changes
  useEffect(() => {
    try {
      localStorage.setItem("track-csv-data", JSON.stringify(csvData));
    } catch (e) {
      console.error("Failed to save track CSV data:", e);
    }
  }, [csvData]);

  useEffect(() => {
    const subscription = constantForm.watch((data) => {
      saveFormData("track-params", data as Record<string, unknown>);
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

  async function onSubmit(data: z.infer<typeof TrackFormSchema>) {
    setIsSubmitting(true);
    try {
      console.log("Form Data:", data);
      console.log("CSV Data:", csvData);

      // Send using YOUR exact variable names - NO CONVERSION
      const trackParams: Record<string, number | number[]> = {
        n_station: data.n_station,
        x_station: data.x_station,
        radius: data.radius,
        slope: data.slope,
        v_limit: data.v_limit,
        dwellTime: data.dwellTime,
        slope_option1: data.slope_option1,
        slope_option2: data.slope_option2,
        slope_option3: data.slope_option3,
        slope_option4: data.slope_option4,
      };

      // Add CSV array data if available
      if (csvData.x_station && csvData.x_station.length > 0) {
        // Original logic: Col 2 is x_station (Segment), Col 1 is tot_x_station (Cumulative)
        const stationData = csvData.x_station;
        if (stationData[0].length >= 3) {
          trackParams.x_station_array = stationData.map((row) => row[2]);
          trackParams.tot_x_station_array = stationData.map((row) => row[1]);
        } else {
          // Fallback if user uploads single column (unlikely given new constraint but safe)
          trackParams.x_station_array = csvData.x_station.flat();
        }
      }

      // Handle 3-column CSVs for Speed Limit, Slope, Radius
      // Column 0: Start, Column 1: End, Column 2: Value
      if (csvData.v_limit && csvData.v_limit.length > 0) {
        const v_limitData = csvData.v_limit;
        if (v_limitData[0].length >= 3) {
          trackParams.x_v_limitStart_array = v_limitData.map((row) => row[0]);
          trackParams.x_v_limitEnd_array = v_limitData.map((row) => row[1]);
          trackParams.v_limit_array = v_limitData.map((row) => row[2]);
        } else {
          trackParams.v_limit_array = v_limitData.flat();
        }
      }

      if (csvData.slope && csvData.slope.length > 0) {
        const slopeData = csvData.slope;
        if (slopeData[0].length >= 3) {
          trackParams.x_slopeStart_array = slopeData.map((row) => row[0]);
          trackParams.x_slopeEnd_array = slopeData.map((row) => row[1]);
          trackParams.slope_array = slopeData.map((row) => row[2]);
        } else {
          trackParams.slope_array = slopeData.flat();
        }
      }

      if (csvData.radius && csvData.radius.length > 0) {
        const radiusData = csvData.radius;
        if (radiusData[0].length >= 3) {
          trackParams.x_radiusStart_array = radiusData.map((row) => row[0]);
          trackParams.x_radiusEnd_array = radiusData.map((row) => row[1]);
          trackParams.radius_array = radiusData.map((row) => row[2]);
        } else {
          trackParams.radius_array = radiusData.flat();
        }
      }

      if (csvData.dwellTime && csvData.dwellTime.length > 0) {
        trackParams.dwellTime_array = csvData.dwellTime.flat();
      }

      const result = await api.updateTrackParameters(trackParams);
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
    clearFormData("track-params");
    setCsvData({});
    localStorage.removeItem("track-csv-data");
    toast(trans("toast.reset"));
  };

  /**
   * Handle CSV File Upload (Batch Config)
   *
   * Expected CSV Format (Simple Key-Value):
   * ---------------------------------------
   * n_station,2
   * x_station,2000
   * radius,300
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
    const validKeys = Object.keys(TrackFormSchema.shape);
    lines.forEach((line) => {
      if (!line.trim()) return;
      const [key, valueStr] = line.split(/,(.+)/);
      const cleanKey = key?.trim();
      const cleanValue = valueStr?.trim();
      if (!cleanKey || !cleanValue) return;
      if (validKeys.includes(cleanKey) && !isNaN(Number(cleanValue))) {
        constantForm.setValue(
          cleanKey as keyof z.infer<typeof TrackFormSchema>,
          Number(cleanValue),
          { shouldDirty: true, shouldValidate: true },
        );
        successCount++;
      } else {
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
                          "Select Track Parameters CSV File",
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
      <Card className="px-6 py-8 min-h-[40rem] h-fit w-full max-w-2xl rounded-3xl justify-center">
        <CardHeader className="text-center">
          <CardTitle className="text-2xl">{trans("slopeTitle")}</CardTitle>
          <CardDescription>
            {trans("slopeDescription")}
          </CardDescription>
        </CardHeader>
        <CardContent>
          <Form {...constantForm}>
            <form
              onSubmit={constantForm.handleSubmit(onSubmit)}
              className="space-y-6"
            >
              <div className="flex flex-col gap-6">
                {slopeFormRows.map((row, rowIndex) => (
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
                          "Select Track Parameters CSV File",
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
