'use client';

import { InputWidget } from '@/components/inputs/input-widget';
import PageLayout from '@/components/page-layout';
import {
  Card,
  CardHeader,
  CardTitle,
  CardDescription,
  CardContent,
} from '@/components/ui/card';
import { useForm } from 'react-hook-form';
import {
  constantFormRows,
  RunningFormSchema,
  CV,
  siNonSiPairs,
  gearComputedFields,
  parseGearNumber,
} from './form.constants';
import { useState, useEffect, useRef } from 'react';
import z from 'zod';
import { zodResolver } from '@hookform/resolvers/zod';
import { toast } from 'sonner';
import { Form } from '@/components/ui/form';
import { api } from '@/services/api';
import { initializeBackendOnce } from '@/lib/backendInit';
import { useFormPersistence } from '@/contexts/FormPersistenceContext';
import { useTranslations } from 'next-intl';
import { exportConfigToCsv } from '@/lib/csv-export';
import { FormActionButtons } from '@/components/buttons/form-action-buttons';

type RunningFormValues = z.infer<typeof RunningFormSchema>;

// Helper: compute effective acceleration/deceleration from max and gear
function computeEffective(max: number, gearStr: string): number {
  const gear = parseGearNumber(gearStr);
  return parseFloat(((max * gear) / 7).toFixed(4));
}

export default function RunningPage() {
  const trans = useTranslations('RunningParams');
  const [isSubmitting, setIsSubmitting] = useState(false);
  const { saveFormData, loadFormData, clearFormData } = useFormPersistence();

  // Guard to prevent infinite sync loops
  const isSyncing = useRef(false);

  const defaultValues: RunningFormValues = {
    startRes: 39.2,
    v_diffCoast: 5,
    acc_linear_si: 0.8,
    acc_linear: 2.88,
    pow_gear: 'P7',
    acc_start_si: 0.8, // 0.8 × 7/7 = 0.8
    acc_start: 2.88, // 2.88 × 7/7 = 2.88
    v_p1: 35,
    v_p2: 65,
    decc_linear_si: 1.078,
    decc_linear: 3.88,
    brake_gear: 'B7',
    decc_start_si: 1.078, // 1.078 × 7/7 = 1.078
    decc_start: 3.88, // 3.88 × 7/7 = 3.88
    decc_emergency_si: 1.43,
    decc_emergency: 5.16,
    v_b1: 55,
    v_b2: 70,
  };

  const constantForm = useForm<RunningFormValues>({
    resolver: zodResolver(RunningFormSchema),
    defaultValues,
  });

  // ─── Recalculate all gear-derived fields from current form data ──────────
  function recalcGearFields(
    data: Partial<RunningFormValues>,
    form: ReturnType<typeof useForm<RunningFormValues>>,
  ) {
    for (const [maxSi, max, gearField, effSi, eff] of gearComputedFields) {
      const maxSiVal = data[maxSi as keyof RunningFormValues] as
        | number
        | undefined;
      const maxVal = data[max as keyof RunningFormValues] as number | undefined;
      const gearStr = data[gearField as keyof RunningFormValues] as
        | string
        | undefined;

      if (maxSiVal !== undefined && gearStr !== undefined) {
        form.setValue(
          effSi as keyof RunningFormValues,
          computeEffective(maxSiVal, gearStr),
          { shouldValidate: true },
        );
      }
      if (maxVal !== undefined && gearStr !== undefined) {
        form.setValue(
          eff as keyof RunningFormValues,
          parseFloat(computeEffective(maxVal, gearStr).toFixed(2)),
          { shouldValidate: true },
        );
      }
    }
  }

  useEffect(() => {
    const savedData = loadFormData('running-params');
    const hasSavedData = savedData && Object.keys(savedData).length > 0;

    if (hasSavedData) {
      const merged = {
        ...defaultValues,
        ...(savedData as RunningFormValues),
      };
      constantForm.reset(merged);
      return;
    }

    const loadDefaults = async () => {
      try {
        await initializeBackendOnce();
        const data = await api.getRunningParameters();
        const params = data.runningParameters;

        // Backend sends acc_start_si/decc_start_si — derive max values
        // (Assume P7/B7 if no gear info from backend)
        const merged: RunningFormValues = {
          ...defaultValues,
          ...params,
          acc_linear_si:
            params.acc_linear_si ??
            params.acc_start_si ??
            defaultValues.acc_linear_si,
          acc_linear: parseFloat(
            (
              (params.acc_linear_si ??
                params.acc_start_si ??
                defaultValues.acc_linear_si) * CV
            ).toFixed(2),
          ),
          decc_linear_si:
            params.decc_linear_si ??
            params.decc_start_si ??
            defaultValues.decc_linear_si,
          decc_linear: parseFloat(
            (
              (params.decc_linear_si ??
                params.decc_start_si ??
                defaultValues.decc_linear_si) * CV
            ).toFixed(2),
          ),
          decc_emergency: parseFloat(
            (
              (params.decc_emergency_si ?? defaultValues.decc_emergency_si) * CV
            ).toFixed(2),
          ),
          pow_gear: params.pow_gear ?? defaultValues.pow_gear,
          brake_gear: params.brake_gear ?? defaultValues.brake_gear,
        };

        // Compute effective values from max + gear
        const gearNum = parseGearNumber(merged.pow_gear);
        merged.acc_start_si = parseFloat(
          (merged.acc_linear_si * (Math.abs(gearNum - 8) / 7)).toFixed(4),
        );
        merged.acc_start = parseFloat(
          (merged.acc_linear * (Math.abs(gearNum - 8) / 7)).toFixed(2),
        );

        const brakeGearNum = parseGearNumber(merged.brake_gear);
        merged.decc_start_si = parseFloat(
          (merged.decc_linear_si * (Math.abs(brakeGearNum - 8) / 7)).toFixed(4),
        );
        merged.decc_start = parseFloat(
          (merged.decc_linear * (Math.abs(brakeGearNum - 8) / 7)).toFixed(2),
        );

        constantForm.reset(merged);
      } catch (err) {
        console.error('Failed to load running parameters:', err);
        toast.error(trans('uploadCsvFailed'));
        constantForm.reset(defaultValues);
      }
    };

    loadDefaults();
  }, []); // eslint-disable-line react-hooks/exhaustive-deps

  // ─── Watchers: SI↔non-SI sync + gear recalculation ─────────────────────
  useEffect(() => {
    const subscription = constantForm.watch((data, { name }) => {
      // Persist form data on every change
      saveFormData('running-params', data as Record<string, unknown>);

      // Only sync if the change was triggered by a specific field (not a reset)
      if (!name || isSyncing.current) return;

      isSyncing.current = true;

      // 1) Bidirectional SI ↔ non-SI sync for max fields
      for (const [siField, nonSiField] of siNonSiPairs) {
        if (name === siField) {
          const siVal = data[siField as keyof typeof data] as
            | number
            | undefined;
          if (siVal !== undefined && !isNaN(siVal)) {
            constantForm.setValue(
              nonSiField as keyof RunningFormValues,
              parseFloat((siVal * CV).toFixed(2)),
              { shouldValidate: true },
            );
          }
          break;
        }
        if (name === nonSiField) {
          const nonSiVal = data[nonSiField as keyof typeof data] as
            | number
            | undefined;
          if (nonSiVal !== undefined && !isNaN(nonSiVal)) {
            constantForm.setValue(
              siField as keyof RunningFormValues,
              parseFloat((nonSiVal / CV).toFixed(4)),
              { shouldValidate: true },
            );
          }
          break;
        }
      }

      // 2) Recalculate effective fields whenever max or gear changes
      const gearTriggerFields = gearComputedFields.flatMap(
        ([maxSi, max, gear]) => [maxSi, max, gear],
      );
      if (gearTriggerFields.includes(name)) {
        // Re-read latest data after SI↔non-SI sync may have updated values
        const current = constantForm.getValues();
        recalcGearFields(current, constantForm);
      }

      isSyncing.current = false;
    });
    return () => subscription.unsubscribe();
  }, [constantForm, saveFormData]);

  async function onSubmit(data: RunningFormValues) {
    setIsSubmitting(true);
    try {
      console.log('Form Data:', data);

      // Send all fields including computed acc_start_si/decc_start_si to backend
      const result = await api.updateRunningParameters(data);
      console.log('Backend response:', result);
      toast.success(trans('toast.success'), {
        description: trans('toast.successDescription'),
      });
    } catch (error) {
      console.error('Error updating parameters:', error);
      toast.error(trans('toast.error'), {
        description: trans('toast.errorDescription'),
      });
    } finally {
      setIsSubmitting(false);
    }
  }

  const processCsvText = (text: string) => {
    console.log('📂 processing CSV upload...');
    const lines = text.split(/\r\n|\n/);
    let successCount = 0;
    let errorCount = 0;

    lines.forEach((line) => {
      if (!line.trim()) return;
      const [key, valueStr] = line.split(/,(.+)/);
      const cleanKey = key?.trim();
      const cleanValue = valueStr?.trim();
      if (!cleanKey || !cleanValue) return;

      const validKeys: (keyof RunningFormValues)[] = [
        'startRes',
        'v_diffCoast',
        'acc_linear_si',
        'acc_linear',
        'pow_gear',
        'acc_start_si',
        'acc_start',
        'v_p1',
        'v_p2',
        'decc_linear_si',
        'decc_linear',
        'brake_gear',
        'decc_start_si',
        'decc_start',
        'decc_emergency_si',
        'decc_emergency',
        'v_b1',
        'v_b2',
      ];

      if (
        validKeys.includes(cleanKey as keyof RunningFormValues) &&
        !isNaN(Number(cleanValue))
      ) {
        constantForm.setValue(
          cleanKey as keyof RunningFormValues,
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
    clearFormData('running-params');
    toast(trans('toast.reset'));
  };

  return (
    <PageLayout>
      <Card className='px-6 py-8 min-h-[40rem] h-full w-full max-w-3xl rounded-3xl justify-center overflow-auto'>
        <CardHeader className='text-center'>
          <CardTitle className='text-2xl'>{trans('title')}</CardTitle>
          <CardDescription>{trans('description')}</CardDescription>
        </CardHeader>
        <CardContent>
          <Form {...constantForm}>
            <form
              onSubmit={constantForm.handleSubmit(onSubmit)}
              className='space-y-6'>
              <div className='flex flex-col gap-6'>
                {constantFormRows.map((row, rowIndex) => (
                  <div key={rowIndex} className='grid grid-cols-3 gap-6 justify-end'>
                    {row.map((inputType) =>
                      inputType.name.startsWith('_spacer') ? (
                        <div key={inputType.name} />
                      ) : (
                        <InputWidget
                          key={inputType.name}
                          inputType={inputType}
                          control={constantForm.control}
                        />
                      ),
                    )}
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
                    'running-parameters.csv',
                    trans('exportSuccess'),
                  )
                }
                dialogTitle='Select Running Parameters CSV File'
                labels={{
                  save: trans('save'),
                  saving: trans('saving'),
                  uploadCsv: trans('uploadCsv'),
                  uploading: trans('uploading'),
                  reset: trans('reset'),
                  exportCsv: trans('exportCsv'),
                }}
              />
            </form>
          </Form>
        </CardContent>
      </Card>
    </PageLayout>
  );
}
