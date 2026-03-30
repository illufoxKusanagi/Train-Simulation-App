import { Button } from "@/components/ui/button";
import { Spinner } from "@/components/ui/spinner";
import { isQtWebChannelReady, openFileWithDialog } from "@/lib/qt-webchannel";
import { useRef, useState } from "react";

export interface FormActionLabels {
  save: string;
  saving: string;
  uploadCsv: string;
  uploading: string;
  reset: string;
  exportCsv: string;
}

interface FormActionButtonsProps {
  isSubmitting: boolean;
  onProcessCsvText: (text: string) => void;
  onReset: () => void;
  onExport: () => void;
  dialogTitle: string;
  labels: FormActionLabels;
}

export function FormActionButtons({
  isSubmitting,
  onProcessCsvText,
  onReset,
  onExport,
  dialogTitle,
  labels,
}: FormActionButtonsProps) {
  const [isUploading, setIsUploading] = useState(false);
  const csvInputRef = useRef<HTMLInputElement>(null);

  const handleCsvUpload = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (!file) return;
    const reader = new FileReader();

    reader.onload = (e) => {
      const text = e.target?.result as string;
      if (!text) return;
      onProcessCsvText(text);
      event.target.value = ""; // Clear input so same file can be selected again
    };
    reader.readAsText(file);
  };

  const handleUploadClick = async () => {
    if (isQtWebChannelReady()) {
      setIsUploading(true);
      const result = await openFileWithDialog(
        dialogTitle,
        "CSV Files (*.csv);;All Files (*)",
      );
      if (result.success && result.content) {
        onProcessCsvText(result.content);
      }
      setIsUploading(false);
    } else {
      csvInputRef.current?.click();
    }
  };

  return (
    <div className="flex flex-col gap-4 pt-4">
      <div className="grid grid-cols-2 gap-4">
        <Button type="submit" className="flex-1" disabled={isSubmitting}>
          {isSubmitting ? (
            <>
              <Spinner className="mr-2" />
              {labels.saving}
            </>
          ) : (
            labels.save
          )}
        </Button>
        <Button
          type="button"
          variant="outline"
          className="flex-1"
          onClick={onReset}
        >
          {labels.reset}
        </Button>
      </div>
      <div className="grid grid-cols-2 gap-4">
        <div className="flex-1 relative">
          <input
            ref={csvInputRef}
            type="file"
            accept=".csv"
            className="hidden"
            onChange={handleCsvUpload}
          />
          <Button
            type="button"
            variant="default"
            className="w-full"
            disabled={isUploading}
            onClick={handleUploadClick}
          >
            {isUploading ? (
              <>
                <Spinner className="mr-2" />
                {labels.uploading}
              </>
            ) : (
              labels.uploadCsv
            )}
          </Button>
        </div>

        <Button
          type="button"
          variant="outline"
          className="flex-1"
          onClick={onExport}
        >
          {labels.exportCsv}
        </Button>
      </div>
    </div>
  );
}
