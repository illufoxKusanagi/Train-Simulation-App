"use client";

import { Form } from "@/components/ui/form";
import { useForm } from "react-hook-form";
import { z } from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { toast } from "sonner";
import { Button } from "@/components/ui/button";
import { useEffect, useState, useCallback } from "react";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { InputWidget } from "@/components/inputs/input-widget";
import {
  CalculatedMassFormSchema,
  calculatedMassRows,
  carMassFormRows,
  carPassangerFormRows,
  carTypeFormRows,
  constantFormRows,
  ConstantFormSchema,
  trainsetFormRows,
  TrainsetFormSchema,
} from "./form.constant";
import { cn } from "@/lib/utils";
import PageLayout from "@/components/page-layout";
import { api } from "@/services/api";
import { useFormPersistence } from "@/contexts/FormPersistenceContext";

// Preset configurations for n_car dropdown (outside component to avoid recreating on every render)
const carPresets: Record<
  string | number,
  {
    n_M1: number;
    n_M2: number;
    n_Tc: number;
    n_T1: number;
    n_T2: number;
    n_T3: number;
    n_M1_disabled: number;
    n_M2_disabled: number;
  }
> = {
  "12": {
    n_M1: 3,
    n_M2: 3,
    n_Tc: 2,
    n_T1: 2,
    n_T2: 1,
    n_T3: 1,
    n_M1_disabled: 0,
    n_M2_disabled: 0,
  },
  "10": {
    n_M1: 3,
    n_M2: 2,
    n_Tc: 2,
    n_T1: 2,
    n_T2: 1,
    n_T3: 0,
    n_M1_disabled: 0,
    n_M2_disabled: 0,
  },
  "8": {
    n_M1: 2,
    n_M2: 2,
    n_Tc: 2,
    n_T1: 1,
    n_T2: 1,
    n_T3: 0,
    n_M1_disabled: 0,
    n_M2_disabled: 0,
  },
  "6": {
    n_M1: 2,
    n_M2: 1,
    n_Tc: 2,
    n_T1: 1,
    n_T2: 0,
    n_T3: 0,
    n_M1_disabled: 0,
    n_M2_disabled: 0,
  },
  "4": {
    n_M1: 1,
    n_M2: 1,
    n_Tc: 2,
    n_T1: 0,
    n_T2: 0,
    n_T3: 0,
    n_M1_disabled: 0,
    n_M2_disabled: 0,
  },
  "12-Degraded": {
    n_M1: 3,
    n_M2: 3,
    n_Tc: 2,
    n_T1: 2,
    n_T2: 1,
    n_T3: 1,
    n_M1_disabled: 1,
    n_M2_disabled: 1,
  },
  "10-Degraded": {
    n_M1: 3,
    n_M2: 2,
    n_Tc: 2,
    n_T1: 2,
    n_T2: 1,
    n_T3: 0,
    n_M1_disabled: 1,
    n_M2_disabled: 1,
  },
};

export default function TrainParameter() {
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [csvData, setCsvData] = useState<Record<string, number[][]>>({});
  const { saveFormData, loadFormData } = useFormPersistence();

  // Load saved form data
  const savedConstantData = loadFormData("train-constant");
  const savedTrainsetData = loadFormData("train-trainset");
  const savedCalculatedData = loadFormData("train-calculated");

  const constantForm = useForm<z.infer<typeof ConstantFormSchema>>({
    resolver: zodResolver(ConstantFormSchema),
    defaultValues: (savedConstantData as z.infer<
      typeof ConstantFormSchema
    >) || {
      i_T: 1.05,
      i_M: 1.1,
      n_axle: 4,
      n_tm: 24,
      wheelDiameter: 860,
      mass_P: 70,
      gearRatio: 3,
      load: 0,
      carLength: 20,
      loadCondition: "AW4",
    },
  });

  const trainsetForm = useForm<z.infer<typeof TrainsetFormSchema>>({
    resolver: zodResolver(TrainsetFormSchema),
    defaultValues: (savedTrainsetData as z.infer<
      typeof TrainsetFormSchema
    >) || {
      n_car: 12,
      n_M1: 2,
      n_M2: 2,
      n_Tc: 2,
      n_T1: 2,
      n_T2: 2,
      n_T3: 2,
      n_M1_disabled: 0,
      n_M2_disabled: 0,
      mass_M1: 20,
      mass_M2: 20,
      mass_Tc: 10,
      mass_T1: 10,
      mass_T2: 10,
      mass_T3: 10,
      n_PM1: 200,
      n_PM2: 200,
      n_PTc: 100,
      n_PT1: 200,
      n_PT2: 200,
      n_PT3: 200,
    },
  });

  const calculatedMassForm = useForm<z.infer<typeof CalculatedMassFormSchema>>({
    resolver: zodResolver(CalculatedMassFormSchema),
    defaultValues: (savedCalculatedData as z.infer<
      typeof CalculatedMassFormSchema
    >) || {
      mass_totalEmpty: 180,
      mass_totalLoad: 334,
      mass_totalInertial: 349,
    },
  });

  // Handler for n_car dropdown changes
  const handleCarNumberChange = useCallback(
    (value: string) => {
      const preset = carPresets[value];
      if (preset) {
        // Update all car type fields with preset values
        trainsetForm.setValue("n_M1", preset.n_M1);
        trainsetForm.setValue("n_M2", preset.n_M2);
        trainsetForm.setValue("n_Tc", preset.n_Tc);
        trainsetForm.setValue("n_T1", preset.n_T1);
        trainsetForm.setValue("n_T2", preset.n_T2);
        trainsetForm.setValue("n_T3", preset.n_T3);
        trainsetForm.setValue("n_M1_disabled", preset.n_M1_disabled);
        trainsetForm.setValue("n_M2_disabled", preset.n_M2_disabled);

        console.log(`Applied preset for ${value}-car configuration:`, preset);
      }
    },
    [trainsetForm]
  );

  const handleFileLoad = (name: string, data: number[][]) => {
    setCsvData((prev) => ({
      ...prev,
      [name]: data,
    }));
    console.log(`File loaded for ${name}:`, data);
  };

  async function onSubmit(data: z.infer<typeof ConstantFormSchema>) {
    setIsSubmitting(true);
    try {
      console.log("Form Data:", data);
      console.log("CSV Data:", csvData);

      const trainsetData = trainsetForm.getValues();

      const trainParams = {
        numberOfMotorCars: data.n_tm,
        numberOfAxles: data.n_axle,
        numberOfCars: trainsetData.n_car,
        gearRatio: data.gearRatio,
        wheelDiameter: data.wheelDiameter, // Keep as-is from form
        trainsetLength: data.carLength * trainsetData.n_car,
        trainLoad: data.load,
        numberOfM1Cars: trainsetData.n_M1,
        numberOfM2Cars: trainsetData.n_M2,
        numberOfTcCars: trainsetData.n_Tc,
        numberOfT1Cars: trainsetData.n_T1,
        numberOfT2Cars: trainsetData.n_T2,
        numberOfT3Cars: trainsetData.n_T3,
        numberOfM1DisabledCars: trainsetData.n_M1_disabled,
        numberOfM2DisabledCars: trainsetData.n_M2_disabled,
      };

      const result = await api.updateTrainParameters(trainParams);
      console.log("Backend response:", result);
      toast.success("Success!", {
        description: "Constant parameters updated successfully",
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

  async function onTrainsetSubmit(data: z.infer<typeof TrainsetFormSchema>) {
    setIsSubmitting(true);
    try {
      console.log("Trainset data submitted:", data);
      console.log("CSV Data:", csvData);

      // 1. Update car number parameters
      const carNumberParams = {
        n_M1: data.n_M1,
        n_M2: data.n_M2,
        n_Tc: data.n_Tc,
        n_T1: data.n_T1,
        n_T2: data.n_T2,
        n_T3: data.n_T3,
        n_M1_disabled: data.n_M1_disabled,
        n_M2_disabled: data.n_M2_disabled,
      };

      await api.updateCarNumberParameters(carNumberParams);

      // 2. Update mass parameters
      const massParams = {
        mass_M1: data.mass_M1,
        mass_M2: data.mass_M2,
        mass_Tc: data.mass_Tc,
        mass_T1: data.mass_T1,
        mass_T2: data.mass_T2,
        mass_T3: data.mass_T3,
      };

      await api.updateMassParameters(massParams);

      // 3. Update passenger parameters
      const passengerParams = {
        n_PTc: data.n_PTc,
        n_PM1: data.n_PM1,
        n_PM2: data.n_PM2,
        n_PT1: data.n_PT1, // Fixed: was n_Pt1 (lowercase t)
        n_PT2: data.n_PT2, // Fixed: was n_Pt2 (lowercase t)
        n_PT3: data.n_PT3, // Fixed: was n_Pt3 (lowercase t)
      };

      await api.updatePassengerParameters(passengerParams);

      toast.success("Success!", {
        description:
          "Trainset configuration (car numbers, masses, passengers) updated successfully",
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

  // Save form data to localStorage whenever it changes
  useEffect(() => {
    const subscription = constantForm.watch((data) => {
      saveFormData("train-constant", data as Record<string, unknown>);
    });
    return () => subscription.unsubscribe();
  }, [constantForm, saveFormData]);

  useEffect(() => {
    const subscription = trainsetForm.watch((data) => {
      saveFormData("train-trainset", data as Record<string, unknown>);
    });
    return () => subscription.unsubscribe();
  }, [trainsetForm, saveFormData]);

  useEffect(() => {
    const subscription = calculatedMassForm.watch((data) => {
      saveFormData("train-calculated", data as Record<string, unknown>);
    });
    return () => subscription.unsubscribe();
  }, [calculatedMassForm, saveFormData]);

  // Watch n_car changes and apply preset configurations
  useEffect(() => {
    const subscription = trainsetForm.watch((value, { name }) => {
      if (name === "n_car" && value.n_car) {
        handleCarNumberChange(value.n_car.toString());
      }
    });

    return () => subscription.unsubscribe();
  }, [trainsetForm, handleCarNumberChange]);

  useEffect(() => {
    // Watch both trainset AND constant form changes
    const trainsetSubscription = trainsetForm.watch(() => recalculateMass());
    const constantSubscription = constantForm.watch(() => recalculateMass());

    // Calculate masses whenever any field changes
    // Calculate masses whenever any field changes
    async function recalculateMass() {
      // Get current values from forms
      const trainsetData = trainsetForm.getValues();
      const constantData = constantForm.getValues();

      try {
        const data = await api.calculateMass(trainsetData, constantData);

        // Update calculated mass form
        calculatedMassForm.setValue(
          "mass_totalEmpty",
          data.massParameters.totalEmptyMass
        );
        calculatedMassForm.setValue(
          "mass_totalLoad",
          data.massParameters.totalLoadMass
        );
        calculatedMassForm.setValue(
          "mass_totalInertial",
          data.massParameters.totalInertialMass
        );
      } catch (error) {
        console.error("Failed to calculate mass:", error);
      }
    }

    // Initial calculation
    recalculateMass();

    return () => {
      trainsetSubscription.unsubscribe();
      constantSubscription.unsubscribe();
    };
  }, [trainsetForm, constantForm, calculatedMassForm]);

  return (
    <PageLayout>
      <Card className="px-6 py-8 max-h-[45rem] min-h-[40rem] h-full w-full max-w-2xl rounded-3xl justify-center">
        <CardHeader className="text-center">
          <CardTitle className="text-2xl">Train Constant Parameter</CardTitle>
          <CardDescription>
            Input related to Train and Car configuration
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
                        onFileLoad={handleFileLoad}
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
      <Card className="px-2 py-8 max-h-[45rem] min-h-[40rem] w-full max-w-2xl rounded-3xl overflow-auto">
        <CardHeader className="text-center">
          <CardTitle className="text-2xl">Trainset Inputs</CardTitle>
          <CardDescription>
            Related to trainset configuration and car information
          </CardDescription>
        </CardHeader>
        <CardContent>
          <Form {...trainsetForm}>
            <form
              onSubmit={trainsetForm.handleSubmit(onTrainsetSubmit)}
              className="space-y-6"
            >
              {trainsetFormRows.map((row, rowIndex) => (
                <div
                  key={rowIndex}
                  className="flex flex-row gap-4 justify-start"
                >
                  {row.map((inputType) => (
                    <InputWidget
                      key={inputType.name}
                      inputType={inputType}
                      control={trainsetForm.control}
                      onFileLoad={handleFileLoad}
                    />
                  ))}
                  <div
                    className={cn(
                      "flexcontainer w-4xl h-28 border rounded-lg hover:bg-accent hover:text-accent-foreground dark:bg-input/30 dark:border-input dark:hover:bg-input/50 justify-center items-center"
                    )}
                  >
                    <p>lorem ipsum</p>
                  </div>
                </div>
              ))}
              <Card>
                <CardHeader>
                  <CardTitle className="text-center">Calculated Mass</CardTitle>
                </CardHeader>
                <CardContent>
                  {calculatedMassRows.map((row, rowIndex) => (
                    <div
                      key={rowIndex}
                      className="flex flex-row gap-4 justify-center"
                    >
                      {row.map((inputType) => (
                        <InputWidget
                          key={inputType.name}
                          inputType={{
                            ...inputType,
                            isReadOnly: true, // Make read-only since it's calculated
                          }}
                          control={calculatedMassForm.control}
                          onFileLoad={handleFileLoad}
                        />
                      ))}
                    </div>
                  ))}
                </CardContent>
              </Card>
              <div className="flex flex-row gap-2">
                <Card>
                  <CardHeader>
                    <CardTitle className="text-center">Car number</CardTitle>
                  </CardHeader>
                  <CardContent>
                    {carTypeFormRows.map((row, rowIndex) => (
                      <div
                        key={rowIndex}
                        className="flex flex-col gap-4 justify-start"
                      >
                        {row.map((inputType) => (
                          <InputWidget
                            key={inputType.name}
                            inputType={inputType}
                            control={trainsetForm.control}
                            onFileLoad={handleFileLoad}
                          />
                        ))}
                      </div>
                    ))}
                  </CardContent>
                </Card>
                <Card>
                  <CardHeader>
                    <CardTitle className="text-center">Car Passenger</CardTitle>
                  </CardHeader>
                  <CardContent>
                    {carPassangerFormRows.map((row, rowIndex) => (
                      <div
                        key={rowIndex}
                        className="flex flex-col gap-4 justify-start"
                      >
                        {row.map((inputType) => (
                          <InputWidget
                            key={inputType.name}
                            inputType={inputType}
                            control={trainsetForm.control}
                            onFileLoad={handleFileLoad}
                          />
                        ))}
                      </div>
                    ))}
                  </CardContent>
                </Card>
                <Card>
                  <CardHeader>
                    <CardTitle className="text-center">Car Mass</CardTitle>
                  </CardHeader>
                  <CardContent>
                    {carMassFormRows.map((row, rowIndex) => (
                      <div
                        key={rowIndex}
                        className="flex flex-col gap-4 justify-start"
                      >
                        {row.map((inputType) => (
                          <InputWidget
                            key={inputType.name}
                            inputType={inputType}
                            control={trainsetForm.control}
                            onFileLoad={handleFileLoad}
                          />
                        ))}
                      </div>
                    ))}
                  </CardContent>
                </Card>
              </div>

              <div className="flex gap-4 pt-4">
                <Button
                  type="submit"
                  className="flex-1"
                  disabled={isSubmitting}
                >
                  {isSubmitting ? "Saving..." : "Save Data"}
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
