import { isQtWebChannelReady, saveFileWithDialog } from "./qt-webchannel";
import { toast } from "sonner";

/**
 * Export a flat key-value record as a CSV file.
 *
 * Each row in the generated CSV has the format: key,value
 *
 * The function first tries the native Qt desktop "Save As" dialog
 * (when running inside Qt WebEngine). If the Qt bridge is unavailable
 * (e.g. during dev in a regular browser), it falls back to the
 * standard Blob-based download.
 */
/**
 * Export an array of objects as a CSV file with column headers.
 *
 * This is meant for tabular data (like optimization results), where
 * every row is an object with the same set of keys.
 */
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

  const headerLine = columns.map((c) => c.header).join(",");
  const dataLines = rows.map((row) =>
    columns.map((c) => row[c.key] ?? "").join(","),
  );
  const csvContent = [headerLine, ...dataLines].join("\n") + "\n";

  // --- Qt native dialog path ---
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

/**
 * Export a flat key-value record as a CSV file.
 *
 * Each row in the generated CSV has the format: key,value
 *
 * The function first tries the native Qt desktop "Save As" dialog
 * (when running inside Qt WebEngine). If the Qt bridge is unavailable
 * (e.g. during dev in a regular browser), it falls back to the
 * standard Blob-based download.
 */
export async function exportConfigToCsv(
  data: Record<string, unknown>,
  defaultFilename: string,
  successMessage = "CSV exported successfully!",
) {
  // Build the CSV string
  const lines = Object.entries(data)
    .filter(([, v]) => v !== undefined && v !== null && v !== "")
    .map(([key, value]) => `${key},${value}`);

  if (lines.length === 0) {
    toast.warning("No data to export");
    return;
  }

  const csvContent = lines.join("\n") + "\n";

  // --- Qt native dialog path ---
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
