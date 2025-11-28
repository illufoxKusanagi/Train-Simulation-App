import { toast as sonnerToast } from "sonner";

// Re-export the native toast for direct usage if needed
export { sonnerToast as nativeToast };

// Convenience methods matching the previous API signature
// This maintains compatibility with existing code: toast.success("Title", "Description")
export const toast = {
  success: (
    title: string,
    descriptionOrOptions?: string | { description?: string; duration?: number }
  ) => {
    const options =
      typeof descriptionOrOptions === "string"
        ? { description: descriptionOrOptions }
        : descriptionOrOptions;
    sonnerToast.success(title, options);
  },

  error: (
    title: string,
    descriptionOrOptions?: string | { description?: string; duration?: number }
  ) => {
    const options =
      typeof descriptionOrOptions === "string"
        ? { description: descriptionOrOptions }
        : descriptionOrOptions;
    sonnerToast.error(title, options);
  },

  warning: (
    title: string,
    descriptionOrOptions?: string | { description?: string; duration?: number }
  ) => {
    const options =
      typeof descriptionOrOptions === "string"
        ? { description: descriptionOrOptions }
        : descriptionOrOptions;
    sonnerToast.warning(title, options);
  },

  info: (
    title: string,
    descriptionOrOptions?: string | { description?: string; duration?: number }
  ) => {
    const options =
      typeof descriptionOrOptions === "string"
        ? { description: descriptionOrOptions }
        : descriptionOrOptions;
    sonnerToast.info(title, options);
  },

  // Add a generic message method if needed
  message: (
    title: string,
    descriptionOrOptions?: string | { description?: string; duration?: number }
  ) => {
    const options =
      typeof descriptionOrOptions === "string"
        ? { description: descriptionOrOptions }
        : descriptionOrOptions;
    sonnerToast.message(title, options);
  },
};
