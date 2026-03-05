import z from "zod";
import { InputType } from "../../types/input-types";

const chunkArray = <T>(array: T[], chunkSize: number): T[][] => {
  const chunks: T[][] = [];
  for (let i = 0; i < array.length; i += chunkSize) {
    chunks.push(array.slice(i, i + chunkSize));
  }
  return chunks;
};

export const TrackFormSchema = z.object({
  n_station: z.coerce
    .number<number>({
      message: "This Value must be a number",
    })
    .min(2, { message: "Must be at least 2 stations" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  x_station: z.coerce
    .number<number>({
      message: "This Value must be a number",
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  radius: z.coerce
    .number<number>({
      message: "This Value must be a number",
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  slope: z.coerce
    .number<number>({
      message: "This Value must be a number",
    })
    .min(-50, { message: "Value cannot less than -50" })
    .max(50, { message: "Value cannot exceed than 50" }),
  v_limit: z.coerce
    .number<number>({
      message: "This Value must be a number",
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
  dwellTime: z.coerce
    .number<number>({
      message: "This Value must be a number",
    })
    .min(0, { message: "Value must be non-negative" })
    .max(5000, { message: "Value cannot exceed 5000" }),
});

export const constantInputFormDatas: InputType[] = [
  {
    label: "Number of Station",
    unit: "",
    type: "field",
    name: "n_station",
  },
  {
    label: "Radius per Section",
    unit: "m",
    type: "field-upload",
    name: "radius",
  },
  {
    label: "Station Distances",
    unit: "m",
    type: "field-upload",
    name: "x_station",
    requiredColumns: 3,
  },
  {
    label: "Slope per Section",
    unit: "‰",
    type: "field-upload",
    name: "slope",
  },
  {
    label: "Speed Limit per Section",
    unit: "km/h",
    type: "field-upload",
    name: "v_limit",
  },
  {
    label: "Dwell Time at Stations",
    unit: "s",
    type: "field-upload",
    name: "dwellTime",
  },
];

export const constantFormRows = chunkArray(constantInputFormDatas, 2);
