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
import { useState } from "react";
import { useForm } from "react-hook-form";
import { toast } from "sonner";
import z from "zod";
import { constantFormRows, ElectricalFormSchema } from "./form.constants";
import { Form } from "@/components/ui/form";
import { api } from "@/services/api";

export default function ElectricalParameterPage() {
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [csvData, setCsvData] = useState<Record<string, number[][]>>({});

  const constantForm = useForm<z.infer<typeof ElectricalFormSchema>>({
    resolver: zodResolver(ElectricalFormSchema),
    defaultValues: {
      stat_vol_line: 1500,
      stat_vol_motor: 750,
      stat_pf: 0,
      stat_eff_gear: 95,
      stat_eff_motor: 90,
      stat_eff_vvvf: 90,
      p_aps: 300,
    },
  });

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
      toast.success("Data berhasil disimpan!", {
        description: "Electrical parameters updated successfully",
      });
    } catch (error) {
      console.error("Error updating parameters:", error);
      toast.error("Error!", {
        description: "Gagal menyimpan data. Silakan coba lagi.",
      });
    } finally {
      setIsSubmitting(false);
    }
  }

  const handleReset = () => {
    constantForm.reset();
    setCsvData({});
    toast("Form berhasil direset!");
  };

  return (
    <PageLayout>
      <Card className="px-6 py-8 min-h-[40rem] h-full w-full max-w-2xl rounded-3xl justify-center">
        <CardHeader className="text-center">
          <CardTitle className="text-2xl">Electrical Parameter</CardTitle>
          <CardDescription>
            Input related to Electrical configuration
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
                  {isSubmitting ? "Menyimpan..." : "Simpan"}
                </Button>
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
