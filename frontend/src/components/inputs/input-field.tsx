import { useState } from "react";
import { InputFieldProps } from "../../types/input-types";
import {
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage,
} from "../ui/form";
import { Input } from "../ui/input";

export function InputField({
  label,
  unit,
  name,
  placeholder = "enter value...",
  isReadOnly = false,
  control,
}: InputFieldProps) {
  const [rawValue, setRawValue] = useState<string | null>(null);

  return (
    <FormField
      control={control}
      name={name}
      render={({ field }) => (
        <FormItem className="flex flex-col w-full ">
          <FormLabel className="h-8 items-end">{label}</FormLabel>
          <FormControl>
            <div className="flex items-center gap-2">
              <Input
                placeholder={placeholder}
                type="text"
                inputMode="decimal"
                readOnly={isReadOnly}
                className={`flex-1 ${
                  isReadOnly ? "bg-blue-50 border-blue-200" : ""
                }`}
                value={
                  rawValue !== null
                    ? rawValue
                    : field.value === 0
                      ? "0"
                      : (field.value?.toString() ?? "")
                }
                onChange={(e) => {
                  const inputValue = e.target.value;
                  setRawValue(inputValue);

                  if (inputValue === "") {
                    field.onChange(0);
                    return;
                  }

                  // Normalize comma decimal separator (Windows/European locales)
                  const normalized = inputValue.replace(",", ".");

                  // Allow intermediate typing states like "1.", "-0.", "1.0"
                  if (/^-?\d*\.?\d*$/.test(normalized)) {
                    const numericValue = Number(normalized);
                    if (!isNaN(numericValue)) {
                      field.onChange(numericValue);
                    }
                  }
                }}
                onBlur={(e) => {
                  setRawValue(null);
                  field.onBlur();
                }}
                name={field.name}
              />
              {unit && (
                <span className="text-sm text-gray-600 min-w-fit whitespace-nowrap">
                  {unit}
                </span>
              )}
            </div>
          </FormControl>
          <FormMessage />
        </FormItem>
      )}
    />
  );
}

// import { useState } from "react";
// import { InputFieldProps } from "../../types/input-types";
// import {
//   FormControl,
//   FormField,
//   FormItem,
//   FormLabel,
//   FormMessage,
// } from "../ui/form";
// import { Input } from "../ui/input";

// export function InputField({
//   label,
//   unit,
//   name,
//   placeholder = "enter value...",
//   isReadOnly = false,
//   control,
// }: InputFieldProps) {
//   const [rawValue, setRawValue] = useState<string | null>(null);

//   return (
//     <FormField
//       control={control}
//       name={name}
//       render={({ field }) => (
//         <FormItem className="flex flex-col w-full ">
//           <FormLabel className="h-8 items-end">{label}</FormLabel>
//           <FormControl>
//             <div className="flex items-center gap-2">
//               <Input
//                 placeholder={placeholder}
//                 type="text"
//                 inputMode="decimal"
//                 readOnly={isReadOnly}
//                 className={`flex-1 ${
//                   isReadOnly ? "bg-blue-50 border-blue-200" : ""
//                 }`}
//                 value={
//                   rawValue !== null
//                     ? rawValue
//                     : field.value === 0
//                       ? "0"
//                       : (field.value?.toString() ?? "")
//                 }
//                 onChange={(e) => {
//                   const inputValue = e.target.value;
//                   setRawValue(inputValue);

//                   if (inputValue === "") {
//                     field.onChange(0);
//                     return;
//                   }

//                   // Normalize comma decimal separator (Windows/European locales)
//                   const normalized = inputValue.replace(",", ".");

//                   // Allow intermediate typing states like "1.", "-0.", "1.0"
//                   if (/^-?\d*\.?\d*$/.test(normalized)) {
//                     const numericValue = Number(normalized);
//                     if (!isNaN(numericValue)) {
//                       field.onChange(numericValue);
//                     }
//                   }
//                 }}
//                 onBlur={(e) => {
//                   setRawValue(null);
//                   field.onBlur();
//                 }}
//                 name={field.name}
//               />
//               {unit && (
//                 <span className="text-sm text-gray-600 min-w-fit whitespace-nowrap">
//                   {unit}
//                 </span>
//               )}
//             </div>
//           </FormControl>
//           <FormMessage />
//         </FormItem>
//       )}
//     />
//   );
// }
