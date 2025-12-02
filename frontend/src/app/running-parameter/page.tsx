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
import { useState } from "react";
import z from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { toast } from "sonner";
import { Form } from "@/components/ui/form";
import { api } from "@/services/api";

export default function RunningPage() {
  const [isSubmitting, setIsSubmitting] = useState(false);

  const constantForm = useForm<z.infer<typeof RunningFormSchema>>({
    resolver: zodResolver(RunningFormSchema),
    defaultValues: {
      startRes: 39.2,
      v_diffCoast: 5,
      acc_start: 1,
      v_p1: 35,
      v_p2: 65,
      decc_start: 1,
      decc_emergency: 1.2,
      v_b1: 55,
      v_b2: 70,
    },
  });

  async function onSubmit(data: z.infer<typeof RunningFormSchema>) {
    setIsSubmitting(true);
    try {
      console.log("Form Data:", data);

      // Use YOUR exact field names from RunningParams - NO CONVERSION
      const params = {
        startRes: data.startRes,
        v_diffCoast: data.v_diffCoast,
        acc_start: data.acc_start,
        v_p1: data.v_p1,
        v_p2: data.v_p2,
        v_b1: data.v_b1,
        v_b2: data.v_b2,
        decc_start: data.decc_start,
        decc_emergency: data.decc_emergency,
      };

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

  const handleReset = () => {
    constantForm.reset();
    toast("Form berhasil direset!");
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
              </div>
            </form>
          </Form>
        </CardContent>
      </Card>
    </PageLayout>
  );
}
