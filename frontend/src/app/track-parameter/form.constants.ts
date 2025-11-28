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
  n_station: z.coerce.number<number>({
    message: "This Value must be a number",
  }),
  x_station: z.coerce.number<number>({
    message: "This Value must be a number",
  }),
  radius: z.coerce.number<number>({
    message: "This Value must be a number",
  }),
  slope: z.coerce.number<number>({
    message: "This Value must be a number",
  }),
  v_limit: z.coerce.number<number>({
    message: "This Value must be a number",
  }),
  dwellTime: z.coerce.number<number>({
    message: "This Value must be a number",
  }),
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
