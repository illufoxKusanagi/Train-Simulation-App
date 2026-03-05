"use client";

import z from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { constantFormRows, TrackFormSchema } from "./form.constants";
import { useState } from "react";
import { toast } from "sonner";
import { api } from "@/services/api";
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

export default function TrackParameterPage() {
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [csvData, setCsvData] = useState<Record<string, number[][]>>({});

  const constantForm = useForm<z.infer<typeof TrackFormSchema>>({
    resolver: zodResolver(TrackFormSchema),
    defaultValues: {
      n_station: 2,
      x_station: 2000,
      radius: 300,
      slope: 0,
      v_limit: 80,
      dwellTime: 30,
    },
  });

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
      toast.success("Success!", {
        description: "Track parameters updated successfully",
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

  const handleReset = () => {
    constantForm.reset();
    setCsvData({});
    toast("Form has been reset!");
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
            {
              shouldDirty: true,
              shouldValidate: true,
            }
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
      event.target.value = "";
    };
    reader.readAsText(file);
  };

  return (
    <PageLayout>
      <Card className="px-6 py-8 min-h-[40rem] h-full w-full max-w-2xl rounded-3xl justify-center">
        <CardHeader className="text-center">
          <CardTitle className="text-2xl">Track Parameter</CardTitle>
          <CardDescription>
            Input related to Track configuration
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
                        )
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
                  {isSubmitting ? "Saving..." : "Save"}
                </Button>
                <Button
                  type="button"
                  variant="outline"
                  className="flex-1"
                  onClick={handleReset}
                >
                  Reset
                </Button>
                <div className="flex-1 relative">
                  <input
                    type="file"
                    accept=".csv"
                    className="absolute inset-0 w-full h-full opacity-0 cursor-pointer"
                    onChange={handleCsvUpload}
                    title="Upload CSV"
                  />
                  <Button type="button" variant="secondary" className="w-full">
                    Upload CSV
                  </Button>
                </div>
              </div>
            </form>
          </Form>
        </CardContent>
      </Card>
    </PageLayout>
  );
}
