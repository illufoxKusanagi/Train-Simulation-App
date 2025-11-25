import { toast as sonnerToast } from "sonner";

// Re-export the native toast for direct usage if needed
export { sonnerToast as nativeToast };

// Convenience methods matching the previous API signature
// This maintains compatibility with existing code: toast.success("Title", "Description")
export const toast = {
  success: (title: string, description?: string) =>
    sonnerToast.success(title, { description }),

  error: (title: string, description?: string) =>
    sonnerToast.error(title, { description }),

  warning: (title: string, description?: string) =>
    sonnerToast.warning(title, { description }),

  info: (title: string, description?: string) =>
    sonnerToast.info(title, { description }),

  // Add a generic message method if needed
  message: (title: string, description?: string) =>
    sonnerToast.message(title, { description }),
};
