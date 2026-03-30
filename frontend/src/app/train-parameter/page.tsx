"use client";

import { Form } from "@/components/ui/form";
import { useForm } from "react-hook-form";
import { z } from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { toast } from "sonner";
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
import { useTranslations } from "next-intl";
import { exportConfigToCsv } from "@/lib/csv-export";
import { FormActionButtons } from "@/components/buttons/form-action-buttons";

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
  const trans = useTranslations("TrainParams");

  const defaultConstantValues: z.infer<typeof ConstantFormSchema> = {
    i_T: 1.05,
    i_M: 1.1,
    n_axle: 4,
    n_tm: 24,
    wheelDiameter: 860,
    mass_P: 70,
    gearRatio: 6.53,
    load: 0,
    carLength: 20,
    loadCondition: "AW4",
  };

  const defaultTrainsetValues: z.infer<typeof TrainsetFormSchema> = {
    n_car: 12,
    n_M1: 2,
    n_M2: 2,
    n_Tc: 2,
    n_T1: 2,
    n_T2: 2,
    n_T3: 2,
    n_M1_disabled: 0,
    n_M2_disabled: 0,
    mass_M1: 37.5,
    mass_M2: 36.72,
    mass_Tc: 34.48,
    mass_T1: 33.335,
    mass_T2: 30.05,
    mass_T3: 29.66,
    n_PM1: 289,
    n_PM2: 289,
    n_PTc: 253,
    n_PT1: 289,
    n_PT2: 289,
    n_PT3: 289,
  };

  const defaultCalculatedValues: z.infer<typeof CalculatedMassFormSchema> = {
    mass_totalEmpty: 180,
    mass_totalLoad: 334,
    mass_totalInertial: 349,
  };

  const constantForm = useForm<z.infer<typeof ConstantFormSchema>>({
    resolver: zodResolver(ConstantFormSchema),
    defaultValues: defaultConstantValues,
  });

  const trainsetForm = useForm<z.infer<typeof TrainsetFormSchema>>({
    resolver: zodResolver(TrainsetFormSchema),
    defaultValues: defaultTrainsetValues,
  });

  const calculatedMassForm = useForm<z.infer<typeof CalculatedMassFormSchema>>({
    resolver: zodResolver(CalculatedMassFormSchema),
    defaultValues: defaultCalculatedValues,
  });

  // Load saved form data on client mount to avoid hydration mismatch
  useEffect(() => {
    const savedConstantData = loadFormData("train-constant");
    const savedTrainsetData = loadFormData("train-trainset");
    const savedCalculatedData = loadFormData("train-calculated");

    if (savedConstantData && Object.keys(savedConstantData).length > 0) {
      constantForm.reset({
        ...defaultConstantValues,
        ...(savedConstantData as z.infer<typeof ConstantFormSchema>),
      });
    }
    if (savedTrainsetData && Object.keys(savedTrainsetData).length > 0) {
      trainsetForm.reset({
        ...defaultTrainsetValues,
        ...(savedTrainsetData as z.infer<typeof TrainsetFormSchema>),
      });
    }
    if (savedCalculatedData && Object.keys(savedCalculatedData).length > 0) {
      calculatedMassForm.reset({
        ...defaultCalculatedValues,
        ...(savedCalculatedData as z.infer<typeof CalculatedMassFormSchema>),
      });
    }
  }, []); // eslint-disable-line react-hooks/exhaustive-deps

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
    [trainsetForm],
  );

  /**
   * Handle CSV for CONSTANT PARAMETERS
   *
   * Expected CSV Format: Key,Value
   */

  const processConstantCsvText = (text: string) => {
    console.log("📂 processing Constant CSV upload...");
    const lines = text.split(/\r\n|\n/);
    let successCount = 0;
    let errorCount = 0;
    const validKeys = Object.keys(ConstantFormSchema.shape);
    const updates: Partial<z.infer<typeof ConstantFormSchema>> = {};

    lines.forEach((line) => {
      if (!line.trim()) return;
      const [key, valueStr] = line.split(/,(.+)/);
      const cleanKey = key?.trim();
      const cleanValue = valueStr?.trim();
      if (!cleanKey || !cleanValue) return;
      if (validKeys.includes(cleanKey)) {
        if (cleanKey === "loadCondition") {
          let finalAW: "AW0" | "AW1" | "AW2" | "AW3" | "AW4" | null = null;
          if (!isNaN(Number(cleanValue))) {
            const val = Number(cleanValue);
            if (val >= 0 && val <= 4)
              finalAW = `AW${val}` as "AW0" | "AW1" | "AW2" | "AW3" | "AW4";
          } else if (["AW0", "AW1", "AW2", "AW3", "AW4"].includes(cleanValue)) {
            finalAW = cleanValue as "AW0" | "AW1" | "AW2" | "AW3" | "AW4";
          }
          if (finalAW) {
            updates.loadCondition = finalAW;
            successCount++;
          } else errorCount++;
        } else if (!isNaN(Number(cleanValue))) {
          (updates as Record<string, unknown>)[cleanKey] = Number(cleanValue);
          successCount++;
        } else errorCount++;
      } else errorCount++;
    });

    if (successCount > 0) {
      // Apply all updates in one reset so watch subscriptions fire only once,
      // triggering a single api.calculateMass() instead of one per CSV row.
      constantForm.reset({ ...constantForm.getValues(), ...updates });
      toast.success(`Updated ${successCount} constant fields`);
    }
    if (errorCount > 0) toast.warning(`Skipped ${errorCount} invalid items`);
  };

  const processTrainsetCsvText = (text: string) => {
    console.log("📂 processing Trainset CSV upload...");
    const lines = text.split(/\r\n|\n/);
    let successCount = 0;
    let errorCount = 0;
    const validKeys = Object.keys(TrainsetFormSchema.shape);
    const updates: Partial<z.infer<typeof TrainsetFormSchema>> = {};

    lines.forEach((line) => {
      if (!line.trim()) return;
      const [key, valueStr] = line.split(/,(.+)/);
      const cleanKey = key?.trim();
      const cleanValue = valueStr?.trim();
      if (!cleanKey || !cleanValue) return;
      if (validKeys.includes(cleanKey) && !isNaN(Number(cleanValue))) {
        (updates as Record<string, unknown>)[cleanKey] = Number(cleanValue);
        successCount++;
      } else errorCount++;
    });

    if (successCount > 0) {
      // Apply all updates in one reset so watch subscriptions fire only once.
      // CSV values are applied directly — no need to trigger the n_car preset
      // watcher because the CSV already contains the explicit field values.
      trainsetForm.reset({ ...trainsetForm.getValues(), ...updates });
      toast.success(`Updated ${successCount} trainset fields`);
    }
    if (errorCount > 0) toast.warning(`Skipped ${errorCount} invalid items`);
  };

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
      const carsCount =
        typeof trainsetData.n_car === "string"
          ? parseInt(trainsetData.n_car, 10)
          : trainsetData.n_car;

      const trainParams = {
        tractionMotors: data.n_tm,
        axles: data.n_axle,
        cars: carsCount,
        numberOfMotorCars: data.n_tm,
        numberOfAxles: data.n_axle,
        numberOfCars: carsCount,
        gearRatio: data.gearRatio,
        wheelDiameter: data.wheelDiameter, // Keep as-is from form
        carLength: data.carLength,
        trainsetLength: data.carLength * carsCount,
        trainLoad: data.load,
        load: data.load,
        mass_P: data.mass_P,
        i_M: data.i_M,
        i_T: data.i_T,
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

      toast.success(trans("toast.success"), {
        description: trans("toast.trainsetSuccess"),
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
    constantForm.reset();
    setCsvData({});
    toast(trans("toast.reset"));
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

  // AW Presets (Passenger Counts based on Backend Logic)
  // Order: Tc, M1, M2, T1, T2, T3
  const AW_PRESETS: Record<string, { tc: number; other: number }> = {
    AW0: { tc: 0, other: 0 },
    AW1: { tc: 10, other: 20 },
    AW2: { tc: 20, other: 40 },
    AW3: { tc: 50, other: 100 },
    AW4: { tc: 100, other: 200 },
  };

  // Watch loadCondition changes and update PASSENGER parameters
  useEffect(() => {
    const subscription = constantForm.watch((value, { name }) => {
      if (name === "loadCondition" && value.loadCondition) {
        const preset = AW_PRESETS[value.loadCondition];

        if (preset) {
          // Update passenger counts in trainsetForm to match AW condition
          // This aligns with C++ backend logic where AW sets preset passenger inputs.
          // M1, M2, T1, T2, T3 use 'other' value; Tc uses 'tc' value.

          // Check current values to avoid loops if already set
          const currentVals = trainsetForm.getValues();

          // Helper to batch updates if needed, but separate calls are fine
          if (currentVals.n_PTc !== preset.tc)
            trainsetForm.setValue("n_PTc", preset.tc);

          if (currentVals.n_PM1 !== preset.other)
            trainsetForm.setValue("n_PM1", preset.other);
          if (currentVals.n_PM2 !== preset.other)
            trainsetForm.setValue("n_PM2", preset.other);
          if (currentVals.n_PT1 !== preset.other)
            trainsetForm.setValue("n_PT1", preset.other);
          if (currentVals.n_PT2 !== preset.other)
            trainsetForm.setValue("n_PT2", preset.other);
          if (currentVals.n_PT3 !== preset.other)
            trainsetForm.setValue("n_PT3", preset.other);

          // Also ensure "Load per Car" (override) is 0 so passenger counts are used for calculation
          constantForm.setValue("load", 0);

          toast.info(`Applied ${value.loadCondition} passenger configuration`);
        }
      }
    });

    return () => subscription.unsubscribe();
  }, [constantForm, trainsetForm]);

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
          data.massParameters.totalEmptyMass,
        );
        calculatedMassForm.setValue(
          "mass_totalLoad",
          data.massParameters.totalLoadMass,
        );
        calculatedMassForm.setValue(
          "mass_totalInertial",
          data.massParameters.totalInertialMass,
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
      <div className="flex flex-col lg:flex-row h-full gap-4 p-6">
        <Card className="px-6 py-8 max-h-[45rem] min-h-[40rem] h-full w-full max-w-2xl rounded-3xl justify-center">
          <CardHeader className="text-center">
            <CardTitle className="text-2xl">{trans("constantTitle")}</CardTitle>
            <CardDescription>{trans("constantDescription")}</CardDescription>
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
                          ),
                        )}
                    </div>
                  ))}
                </div>

                <FormActionButtons
                  isSubmitting={isSubmitting}
                  onProcessCsvText={processConstantCsvText}
                  onReset={handleReset}
                  onExport={() =>
                    exportConfigToCsv(
                      constantForm.getValues(),
                      "train-constant-parameters.csv",
                      trans("exportSuccess"),
                    )
                  }
                  dialogTitle="Select Train Constant Parameters CSV File"
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

        <Card className="px-2 py-8 max-h-[45rem] min-h-[40rem] w-full max-w-2xl rounded-3xl overflow-auto">
          <CardHeader className="text-center">
            <CardTitle className="text-2xl">{trans("trainsetTitle")}</CardTitle>
            <CardDescription>{trans("trainsetDescription")}</CardDescription>
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
                        "flex container w-4xl h-28 border rounded-lg hover:bg-accent hover:text-accent-foreground dark:bg-input/30 dark:border-input dark:hover:bg-input/50 justify-center items-center overflow-hidden p-2",
                      )}
                    >
                      {(() => {
                        const nCar = trainsetForm.watch("n_car");
                        const carCount = nCar ? parseInt(nCar.toString()) : 0;
                        if ([6, 8, 10, 12, 14].includes(carCount)) {
                          return (
                            // eslint-disable-next-line @next/next/no-img-element
                            <img
                              src={`/images/trains/${carCount}-train.png`}
                              alt={`${carCount}-car`}
                              className="w-full h-full object-contain"
                            />
                          );
                        }
                        return (
                          <p className="text-muted-foreground text-sm">
                            {trans("noDiagram")}
                          </p>
                        );
                      })()}
                    </div>
                  </div>
                ))}
                <Card>
                  <CardHeader>
                    <CardTitle className="text-center">
                      {trans("calculatedMassTitle")}
                    </CardTitle>
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
                      <CardTitle className="text-center">
                        {trans("carNumberTitle")}
                      </CardTitle>
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
                      <CardTitle className="text-center">
                        {trans("carPassengerTitle")}
                      </CardTitle>
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
                      <CardTitle className="text-center">
                        {trans("carMassTitle")}
                      </CardTitle>
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

                <FormActionButtons
                  isSubmitting={isSubmitting}
                  onProcessCsvText={processTrainsetCsvText}
                  onReset={handleReset}
                  onExport={() =>
                    exportConfigToCsv(
                      trainsetForm.getValues(),
                      "train-trainset-parameters.csv",
                      trans("exportSuccess"),
                    )
                  }
                  dialogTitle="Select Trainset Parameters CSV File"
                  labels={{
                    save: trans("saveTrainset"),
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
      </div>
    </PageLayout>
  );
}
