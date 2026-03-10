"use client";

import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import {
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage,
} from "@/components/ui/form";
import { useState, useRef, useEffect } from "react";
import { Upload, File, AlertCircle } from "lucide-react";
import { toast } from "sonner";
import { InputUploadProps } from "../../types/input-types";
import { isQtWebChannelReady, openFileWithDialog } from "@/lib/qt-webchannel";
import type { ControllerRenderProps } from "react-hook-form";

interface UploadContentProps {
  label?: string;
  requiredColumns: number;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  field: ControllerRenderProps<any, any>;
  onFileLoad?: (filePath: string, data: number[][]) => void;
}

function UploadContent({
  label,
  requiredColumns,
  field,
  onFileLoad,
}: UploadContentProps) {
  const [fileName, setFileName] = useState<string>("No file selected");
  const [isLoaded, setIsLoaded] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const fileInputRef = useRef<HTMLInputElement>(null);

  // Restore display state from the persisted form field value
  useEffect(() => {
    if (field.value && field.value.trim() !== "") {
      setFileName(field.value);
      setIsLoaded(true);
    }
  }, [field.value]);

  const validateColumnCount = (row: string[]): boolean => {
    if (requiredColumns > 0 && row.length < requiredColumns) {
      return false;
    }
    return true;
  };

  const parseCsvData = (csvText: string): number[][] => {
    const lines = csvText.split("\n").filter((line) => line.trim() !== "");
    const data: number[][] = [];

    for (let i = 0; i < lines.length; i++) {
      const values = lines[i].split(",").map((val) => val.trim());

      if (!validateColumnCount(values)) {
        throw new Error(
          `Line ${i + 1} doesn't have enough columns (required: ${requiredColumns}, found: ${values.length})`,
        );
      }

      const rowData: number[] = values.map((value) => {
        const numValue = parseFloat(value);
        return isNaN(numValue) ? 0.0 : numValue;
      });

      data.push(rowData);
    }

    return data;
  };

  const handleQtFileOpen = async () => {
    const dialogTitle = label ? `Select ${label} CSV File` : "Select CSV File";
    const result = await openFileWithDialog(
      dialogTitle,
      "CSV Files (*.csv);;All Files (*)",
    );
    if (!result.success || !result.content) {
      if (result.error !== "User cancelled file dialog") {
        toast.error("Upload Error", { description: result.error });
      }
      return;
    }
    try {
      const data = parseCsvData(result.content);
      if (data.length === 0) throw new Error("File contains no valid data");
      const name = result.filename ?? "file.csv";
      setFileName(name);
      setIsLoaded(true);
      setError(null);
      field.onChange(name);
      onFileLoad?.(name, data);
      toast.success("File loaded successfully", {
        description: `${data.length} rows loaded from ${name}`,
      });
    } catch (err) {
      const errorMsg =
        err instanceof Error ? err.message : "Failed to read CSV file";
      setError(errorMsg);
      setFileName("Failed to load file");
      setIsLoaded(false);
      field.onChange("");
      toast.error("Upload Error", { description: errorMsg });
    }
  };

  const handleFileSelect = async (
    event: React.ChangeEvent<HTMLInputElement>,
  ) => {
    const file = event.target.files?.[0];
    if (!file) return;

    if (!file.name.toLowerCase().endsWith(".csv")) {
      const errorMsg = "Please select a CSV file";
      setError(errorMsg);
      setFileName("Invalid file type");
      toast.error("Upload Error", { description: errorMsg });
      return;
    }

    try {
      const text = await file.text();
      const data = parseCsvData(text);

      if (data.length === 0) {
        throw new Error("File contains no valid data");
      }

      setFileName(file.name);
      setIsLoaded(true);
      setError(null);
      field.onChange(file.name);
      onFileLoad?.(file.name, data);

      toast.success("File loaded successfully", {
        description: `${data.length} rows loaded from ${file.name}`,
      });
    } catch (err) {
      const errorMsg =
        err instanceof Error ? err.message : "Failed to read CSV file";
      setError(errorMsg);
      setFileName("Failed to load file");
      setIsLoaded(false);
      field.onChange("");
      toast.error("Upload Error", { description: errorMsg });
    }
  };

  return (
    <div className="flex items-center gap-3">
      <Button
        type="button"
        variant="outline"
        size="lg"
        onClick={(e) => {
          if (isQtWebChannelReady()) {
            e.preventDefault();
            handleQtFileOpen();
          } else {
            fileInputRef.current?.click();
          }
        }}
        className="flex items-center gap-2"
      >
        <Upload size={16} />
        Choose CSV File
      </Button>

      <div className="flex items-center gap-2 flex-1">
        {isLoaded ? (
          <File size={16} className="text-green-600" />
        ) : error ? (
          <AlertCircle size={16} className="text-red-600" />
        ) : (
          <File size={16} className="text-gray-400" />
        )}

        <span
          className={`text-sm ${
            isLoaded
              ? "text-gray-700"
              : error
                ? "text-red-600"
                : "text-gray-500"
          }`}
        >
          {fileName}
        </span>
      </div>

      <Input
        ref={fileInputRef}
        type="file"
        accept=".csv"
        className="hidden"
        onChange={handleFileSelect}
      />
    </div>
  );
}

export function InputUpload({
  label,
  showLabel = true,
  name,
  requiredColumns = 0,
  control,
  onFileLoad,
}: InputUploadProps) {
  return (
    <FormField
      control={control}
      name={name}
      render={({ field }) => (
        <FormItem className="w-full">
          {showLabel && <FormLabel>{label}</FormLabel>}
          <FormControl>
            <UploadContent
              label={label}
              requiredColumns={requiredColumns}
              field={field}
              onFileLoad={onFileLoad}
            />
          </FormControl>
          <FormMessage />
        </FormItem>
      )}
    />
  );
}
