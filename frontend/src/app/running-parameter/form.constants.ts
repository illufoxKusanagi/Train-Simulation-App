import z from "zod";
import { InputType } from "../../types/input-types";

const chunkArray = <T>(array: T[], chunkSize: number): T[][] => {
  const chunks: T[][] = [];
  for (let i = 0; i < array.length; i += chunkSize) {
    chunks.push(array.slice(i, i + chunkSize));
  }
  return chunks;
};

const inputErrorMessage: string = "This Value must be a number";

export const RunningFormSchema = z.object({
  startRes: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  v_diffCoast: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),

  // ── Max acceleration (user input) ─────────────────────────────
  acc_linear_si: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  acc_linear: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(18000, { message: "Value cannot exceed 18000" }),

  // ── Powering gear (P1–P7) ──────────────────────────────────────
  pow_gear: z.string(),

  // ── Effective acceleration (computed = acc_linear × gear/7) ───────
  acc_start_si: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  acc_start: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(18000, { message: "Value cannot exceed 18000" }),

  v_p1: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  v_p2: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),

  // ── Max deceleration (user input) ─────────────────────────────
  decc_linear_si: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  decc_linear: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(18000, { message: "Value cannot exceed 18000" }),

  // ── Braking gear (B1–B7) ──────────────────────────────────────
  brake_gear: z.string(),

  // ── Effective deceleration (computed = decc_linear × gear/7) ─────
  decc_start_si: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  decc_start: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(18000, { message: "Value cannot exceed 18000" }),

  decc_emergency_si: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  decc_emergency: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(18000, { message: "Value cannot exceed 18000" }),
  v_b1: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  v_b2: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
});

const gearOptions = ["P1", "P2", "P3", "P4", "P5", "P6", "P7"];
const brakeGearOptions = ["B1", "B2", "B3", "B4", "B5", "B6", "B7"];

export const constantInputFormDatas: InputType[] = [
  {
    label: "Starting Resistance",
    unit: "",
    type: "field",
    name: "startRes",
  },
  {
    label: "Weakening Point 1 (Powering)",
    unit: "km/h",
    type: "field",
    name: "v_p1",
  },
  {
    label: "Weakening Point 2 (Powering)",
    unit: "km/h",
    type: "field",
    name: "v_p2",
  },
  {
    label: "Difference Coasting Speed",
    unit: "km/h",
    type: "field",
    name: "v_diffCoast",
  },
  {
    label: "Weakening Point 3 (Braking)",
    unit: "km/h",
    type: "field",
    name: "v_b1",
  },
  {
    label: "Weakening Point 4 (Braking)",
    unit: "km/h",
    type: "field",
    name: "v_b2",
  },
  {
    label: "Linear Acceleration (SI)",
    unit: "m/s²",
    type: "field",
    name: "acc_linear_si",
  },
  {
    label: "Acceleration Start (SI)",
    unit: "m/s²",
    type: "field",
    name: "acc_start_si",
    isReadOnly: true,
  },
  {
    label: "Powering Gear",
    type: "dropdown",
    name: "pow_gear",
    options: gearOptions,
  },
  {
    label: "Linear Acceleration",
    unit: "km/h/s",
    type: "field",
    name: "acc_linear",
  },
  {
    label: "Acceleration Start",
    unit: "km/h/s",
    type: "field",
    name: "acc_start",
    isReadOnly: true,
  },
  {
    label: "",
    type: "field",
    name: "_spacer_acc",
    isReadOnly: true,
  },
  {
    label: "Linear Deceleration (SI)",
    unit: "m/s²",
    type: "field",
    name: "decc_linear_si",
  },
  {
    label: "Deceleration Start (SI)",
    unit: "m/s²",
    type: "field",
    name: "decc_start_si",
    isReadOnly: true,
  },
  {
    label: "Braking Gear",
    type: "dropdown",
    name: "brake_gear",
    options: brakeGearOptions,
  },
  {
    label: "Linear Deceleration",
    unit: "km/h/s",
    type: "field",
    name: "decc_linear",
  },
  {
    label: "Deceleration Start",
    unit: "km/h/s",
    type: "field",
    name: "decc_start",
    isReadOnly: true,
  },
  {
    label: "",
    type: "field",
    name: "_spacer_decc",
    isReadOnly: true,
  },
  {
    label: "Emergency Brake Deceleration (SI)",
    unit: "m/s²",
    type: "field",
    name: "decc_emergency_si",
  },
  {
    label: "Emergency Brake Deceleration",
    unit: "km/h/s",
    type: "field",
    name: "decc_emergency",
  },
];

export const constantFormRows = chunkArray(constantInputFormDatas, 3);

// Conversion factor between SI (m/s²) and train units (km/h/s)
export const CV = 3.6;

// Pairs of (SI field, non-SI field) for bidirectional sync (user-editable fields)
export const siNonSiPairs: [string, string][] = [
  ["acc_linear_si", "acc_linear"],
  ["decc_linear_si", "decc_linear"],
  ["decc_emergency_si", "decc_emergency"],
];

// Gear → computed field mappings: [maxSiField, maxField, gearField, effSiField, effField]
export const gearComputedFields: [string, string, string, string, string][] = [
  ["acc_linear_si", "acc_linear", "pow_gear", "acc_start_si", "acc_start"],
  ["decc_linear_si", "decc_linear", "brake_gear", "decc_start_si", "decc_start"],
];

// Extract the gear number from a gear string like "P4" or "B7"
export function parseGearNumber(gearStr: string | undefined): number {
  if (!gearStr) return 7;
  const match = gearStr.match(/\d+/);
  return match ? parseInt(match[0], 10) : 7;
}
