import { InputType } from "@/types/input-types";
import { z } from "zod";

const inputErrorMessage: string = "This Value must be a number";

const membershipField = (max: number) =>
  z.preprocess(
    (v) => (typeof v === "string" && v.trim() === "" ? undefined : v),
    z.coerce
      .number<number>({ error: inputErrorMessage })
      .min(0, { error: "Value must be non-negative" })
      .max(max, { error: `Value cannot exceed ${max}` }),
  );

export const OptimizationFormSchema = z.object({
  // Acceleration membership peaks (m/s²)
  accelLow: membershipField(10),
  accelMedium: membershipField(10),
  accelHigh: membershipField(10),
  // Weakening point membership peaks (km/h)
  weakeningLow: membershipField(500),
  weakeningMedium: membershipField(500),
  weakeningHigh: membershipField(500),
});

export const accelerationFormDatas: InputType[] = [
  { label: "Low", unit: "m/s²", type: "field", name: "accelLow" },
  { label: "Medium", unit: "m/s²", type: "field", name: "accelMedium" },
  { label: "High", unit: "m/s²", type: "field", name: "accelHigh" },
];

export const weakeningFormDatas: InputType[] = [
  { label: "Low", unit: "km/h", type: "field", name: "weakeningLow" },
  { label: "Medium", unit: "km/h", type: "field", name: "weakeningMedium" },
  { label: "High", unit: "km/h", type: "field", name: "weakeningHigh" },
];
