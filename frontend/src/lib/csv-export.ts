import { isQtWebChannelReady, saveFileWithDialog } from "./qt-webchannel";
import { toast } from "sonner";

function escapeCsvValue(value: unknown): string {
  const stringValue = value === null || value === undefined ? "" : String(value);
  const escapedValue = stringValue.replace(/"/g, '""');

  return /[",\n\r]/.test(escapedValue)
    ? `"${escapedValue}"`
    : escapedValue;
}

export async function exportTableToCsv<T>(
  rows: T[],
  columns: { key: keyof T & string; header: string }[],
  defaultFilename: string,
  successMessage = "CSV exported successfully!",
) {
  if (rows.length === 0) {
    toast.warning("No data to export");
    return;
  }

  const headerLine = columns.map((c) => escapeCsvValue(c.header)).join(",");
  const dataLines = rows.map((row) =>
    columns.map((c) => escapeCsvValue(row[c.key] ?? "")).join(","),
  );
  const csvContent = [headerLine, ...dataLines].join("\n") + "\n";

  if (isQtWebChannelReady()) {
    try {
      const result = await saveFileWithDialog(
        csvContent,
        defaultFilename,
        "CSV Files (*.csv);;All Files (*)",
      );
      if (result.success) {
        toast.success(successMessage);
      } else if (result.error) {
        toast.error(result.error);
      }
    } catch (err) {
      toast.error(
        err instanceof Error ? err.message : "Failed to save CSV file",
      );
    }
    return;
  }

  // --- Browser fallback ---
  try {
    const blob = new Blob([csvContent], { type: "text/csv;charset=utf-8;" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = defaultFilename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
    toast.success(successMessage);
  } catch (err) {
    toast.error(
      err instanceof Error ? err.message : "Failed to export CSV file",
    );
  }
}

export async function exportConfigToCsv(
  data: Record<string, unknown>,
  defaultFilename: string,
  successMessage = "CSV exported successfully!",
) {
  const lines = Object.entries(data)
    .filter(([, v]) => v !== undefined && v !== null && v !== "")
    .map(([key, value]) => `${escapeCsvValue(key)},${escapeCsvValue(value)}`);

  if (lines.length === 0) {
    toast.warning("No data to export");
    return;
  }

  const csvContent = lines.join("\n") + "\n";

  if (isQtWebChannelReady()) {
    try {
      const result = await saveFileWithDialog(
        csvContent,
        defaultFilename,
        "CSV Files (*.csv);;All Files (*)",
      );
      if (result.success) {
        toast.success(successMessage);
      } else if (result.error) {
        toast.error(result.error);
      }
    } catch (err) {
      toast.error(
        err instanceof Error ? err.message : "Failed to save CSV file",
      );
    }
    return;
  }

  // --- Browser fallback ---
  try {
    const blob = new Blob([csvContent], { type: "text/csv;charset=utf-8;" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = defaultFilename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
    toast.success(successMessage);
  } catch (err) {
    toast.error(
      err instanceof Error ? err.message : "Failed to export CSV file",
    );
  }
}
