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
  acc_start_si: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
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
  decc_start_si: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  decc_emergency_si: z.coerce
    .number<number>({
      message: inputErrorMessage,
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
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

export const constantInputFormDatas: InputType[] = [
  {
    label: "Starting Resistance",
    unit: "",
    type: "field",
    name: "startRes",
  },
  {
    label: "Difference Coasting Speed",
    unit: "km/h",
    type: "field",
    name: "v_diffCoast",
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
    label: "Acceleration (SI)",
    unit: "m/s²",
    type: "field",
    name: "acc_start_si",
  },
  {
    label: "Acceleration",
    unit: "km/h/s",
    type: "field",
    name: "acc_start",
  },
  {
    label: "Powering Gear",
    type: "field",
    name: "pow_gear",
  },
  {
    label: "Deceleration (SI)",
    unit: "m/s²",
    type: "field",
    name: "decc_start_si",
  },
  {
    label: "Deceleration",
    unit: "km/h/s",
    type: "field",
    name: "decc_start",
  },
  {
    label: "Braking Gear",
    type: "field",
    name: "brake_gear",
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
    name: "decc_emergency_si",
  },
];

export const constantFormRows = chunkArray(constantInputFormDatas, 3);
