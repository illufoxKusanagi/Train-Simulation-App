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

export const ElectricalFormSchema = z.object({
  stat_vol_line: z.coerce.number<number>({
    message: inputErrorMessage,
  }),
  stat_vol_motor: z.coerce.number<number>({
    message: inputErrorMessage,
  }),
  stat_pf: z.coerce.number<number>({
    message: inputErrorMessage,
  }),
  stat_eff_gear: z.coerce.number<number>({
    message: inputErrorMessage,
  }),
  stat_eff_motor: z.coerce.number<number>({
    message: inputErrorMessage,
  }),
  stat_eff_vvvf: z.coerce.number<number>({
    message: inputErrorMessage,
  }),
  p_aps: z.coerce.number<number>({
    message: inputErrorMessage,
  }),
});

export const constantInputFormDatas: InputType[] = [
  {
    label: "Line Voltage",
    unit: "V",
    type: "field-upload",
    name: "stat_vol_line",
  },
  {
    label: "Power Factor of Traction Motor",
    unit: "%",
    type: "field-upload",
    name: "stat_pf",
  },
  {
    label: "Motor Voltage",
    unit: "V",
    type: "field-upload",
    name: "stat_vol_motor",
  },
  {
    label: "Efficiency of Gearbox",
    unit: "%",
    type: "field-upload",
    name: "stat_eff_gear",
  },
  {
    label: "Efficiency of Traction Motor",
    unit: "%",
    type: "field-upload",
    name: "stat_eff_motor",
  },
  {
    label: "Efficiency of VVVF",
    unit: "%",
    type: "field-upload",
    name: "stat_eff_vvvf",
  },
  {
    label: "Auxiliary Power",
    unit: "kW",
    type: "field",
    name: "stat_aps",
  },
];

export const constantFormRows = chunkArray(constantInputFormDatas, 2);
