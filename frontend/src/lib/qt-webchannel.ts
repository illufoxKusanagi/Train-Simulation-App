/*
 * Qt WebChannel Integration for Train Simulation App
 *
 * This script provides the bridge between the Next.js frontend
 * and the Qt WebEngine desktop application, enabling native
 * Qt file dialogs and other desktop integration features.
 */

interface QtTransport {
  webChannelTransport: unknown;
}

interface QtFileBridge {
  saveFileDialog: (
    data: string,
    filename: string,
    filter: string
  ) => Promise<{
    success: boolean;
    filepath?: string;
    error?: string;
  }>;
  saveBinaryFileDialog: (
    data: number[],
    filename: string,
    filter: string
  ) => Promise<{
    success: boolean;
    filepath?: string;
    error?: string;
  }>;
}

interface QWebChannelClass {
  new (
    transport: unknown,
    callback: (channel: { objects?: { fileBridge?: QtFileBridge } }) => void
  ): unknown;
}

declare global {
  interface Window {
    qt: QtTransport;
    fileBridge: QtFileBridge;
    QWebChannel: QWebChannelClass;
  }
}

let webChannelReady = false;

export function initializeQtWebChannel(): Promise<void> {
  return new Promise((resolve, reject) => {
    if (typeof window === "undefined") {
      // Server-side rendering - skip initialization
      resolve();
      return;
    }

    // Check if we're in Qt WebEngine environment
    if (!window.qt || !window.qt.webChannelTransport) {
      console.log("🌐 Not running in Qt WebEngine - WebChannel unavailable");
      resolve();
      return;
    }

    if (webChannelReady) {
      console.log("✅ Qt WebChannel already initialized");
      resolve();
      return;
    }

    console.log("🔌 Initializing Qt WebChannel...");

    try {
      // Load the Qt WebChannel script
      const script = document.createElement("script");
      script.src = "qrc:///qtwebchannel/qwebchannel.js";
      script.onload = () => {
        console.log("📜 Qt WebChannel script loaded");

        if (!window.QWebChannel) {
          console.error("❌ QWebChannel not available after script load");
          reject(new Error("QWebChannel not available"));
          return;
        }

        // Initialize the WebChannel
        new window.QWebChannel(window.qt.webChannelTransport, (channel) => {
          console.log("🎯 Qt WebChannel initialized successfully");
          console.log(
            "📋 Available channel objects:",
            Object.keys(channel.objects || {})
          );

          // Register the file bridge object
          if (channel.objects && channel.objects.fileBridge) {
            window.fileBridge = channel.objects.fileBridge;
            webChannelReady = true;
            console.log("✅ FileBridge registered and ready for use");
            console.log(
              "🔍 FileBridge methods:",
              Object.keys(window.fileBridge)
            );
            resolve();
          } else {
            console.error("❌ FileBridge object not found in WebChannel");
            console.error("📋 Available objects:", channel.objects);
            reject(new Error("FileBridge not available"));
          }
        });
      };

      script.onerror = (error) => {
        console.error("❌ Failed to load Qt WebChannel script:", error);
        reject(new Error("Failed to load Qt WebChannel script"));
      };

      document.head.appendChild(script);
    } catch (error) {
      console.error("❌ Qt WebChannel initialization error:", error);
      reject(error);
    }
  });
}

export function isQtWebChannelReady(): boolean {
  return webChannelReady && !!window.fileBridge;
}

export function getFileBridge(): QtFileBridge {
  if (!isQtWebChannelReady()) {
    throw new Error(
      "Qt WebChannel not ready - call initializeQtWebChannel() first"
    );
  }
  return window.fileBridge;
}

/**
 * Enhanced file bridge with debugging
 */
export async function saveFileWithDialog(
  data: string,
  filename: string,
  filter: string = "All Files (*.*)"
): Promise<{
  success: boolean;
  filepath?: string;
  error?: string;
}> {
  console.log("🔍 saveFileWithDialog called with:", {
    dataLength: data.length,
    filename,
    filter,
    webChannelReady: isQtWebChannelReady(),
    fileBridgeExists: !!window.fileBridge,
  });

  if (!isQtWebChannelReady()) {
    const error = "Qt WebChannel not ready";
    console.error("❌", error);
    return { success: false, error };
  }

  try {
    console.log("🔄 Calling fileBridge.saveFileDialog...");
    const result = await window.fileBridge.saveFileDialog(
      data,
      filename,
      filter
    );
    console.log("📤 FileBridge response:", result);
    return result;
  } catch (error) {
    console.error("💥 FileBridge error:", error);
    return {
      success: false,
      error: error instanceof Error ? error.message : String(error),
    };
  }
}

// Auto-initialize on module load in browser environment
if (typeof window !== "undefined") {
  // Wait for DOM to be ready
  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", () => {
      initializeQtWebChannel().catch(console.error);
    });
  } else {
    // DOM already ready
    initializeQtWebChannel().catch(console.error);
  }
}

const qtWebChannelExports = {
  initializeQtWebChannel,
  isQtWebChannelReady,
  getFileBridge,
  saveFileWithDialog,
};

export default qtWebChannelExports;
