import { toast as sonnerToast } from "sonner";
import { CheckCircle2, XCircle, AlertTriangle, Info } from "lucide-react";

type ToastType = "success" | "error" | "warning" | "info";

interface ToastOptions {
  title: string;
  description?: string;
  duration?: number;
}

const iconMap = {
  success: CheckCircle2,
  error: XCircle,
  warning: AlertTriangle,
  info: Info,
};

const colorMap = {
  success: "text-green-600",
  error: "text-red-600",
  warning: "text-yellow-600",
  info: "text-blue-600",
};

export const showToast = (type: ToastType, options: ToastOptions) => {
  const Icon = iconMap[type];
  const iconColor = colorMap[type];

  sonnerToast.custom(
    (t) => (
      <div className="bg-white dark:bg-gray-800 border border-gray-200 dark:border-gray-700 rounded-lg shadow-lg p-4 flex items-start gap-3 min-w-[300px] max-w-[500px]">
        <Icon className={`h-5 w-5 mt-0.5 flex-shrink-0 ${iconColor}`} />
        <div className="flex-1">
          <div className="font-semibold text-gray-900 dark:text-gray-100">
            {options.title}
          </div>
          {options.description && (
            <div className="text-sm text-gray-600 dark:text-gray-400 mt-1">
              {options.description}
            </div>
          )}
        </div>
        <button
          onClick={() => sonnerToast.dismiss(t)}
          className="text-gray-400 hover:text-gray-600 dark:hover:text-gray-200"
        >
          ×
        </button>
      </div>
    ),
    {
      duration: options.duration ?? 4000,
    }
  );
};

// Convenience methods matching Qt MessageBox
export const toast = {
  success: (title: string, description?: string) =>
    showToast("success", { title, description }),
  error: (title: string, description?: string) =>
    showToast("error", { title, description }),
  warning: (title: string, description?: string) =>
    showToast("warning", { title, description }),
  info: (title: string, description?: string) =>
    showToast("info", { title, description }),
};
