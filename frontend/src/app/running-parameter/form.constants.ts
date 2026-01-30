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
  acc_start: z.coerce
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
  decc_start: z.coerce
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
    label: "Differentce Coasting Speed",
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
    label: "Acceleration",
    unit: "m/s²",
    type: "field",
    name: "acc_start",
  },
  {
    label: "Deceleration",
    unit: "m/s²",
    type: "field",
    name: "decc_start",
  },
  {
    label: "Emergency Brake Deceleration",
    unit: "m/s²",
    type: "field",
    name: "decc_emergency",
  },
];

export const constantFormRows = chunkArray(constantInputFormDatas, 3);
