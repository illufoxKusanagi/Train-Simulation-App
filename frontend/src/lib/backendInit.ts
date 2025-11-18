// Backend initialization utilities
let isInitialized = false;

export const initializeBackendOnce = async () => {
  if (isInitialized) {
    console.log("Backend already initialized, skipping...");
    return;
  }

  try {
    console.log("Initializing backend with quick setup...");
    const response = await fetch("http://localhost:8080/api/init/quick", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
    });

    if (!response.ok) {
      throw new Error(`Backend init failed: ${response.status}`);
    }

    const result = await response.json();
    console.log("Backend initialized successfully:", result);
    isInitialized = true;
    return result;
  } catch (error) {
    console.error("Failed to initialize backend:", error);
    throw error;
  }
};

export const resetInitialization = () => {
  isInitialized = false;
};
