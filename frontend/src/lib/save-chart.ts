import { toPng } from "html-to-image";

/**
 * Captures a chart element as a PNG data URL.
 *
 * html-to-image clones the DOM into a hidden iframe, which loses two things:
 *   1. CSS custom properties from the parent document's :root (e.g. --chart-1
 *      used by ChartContainer to set line/fill colors).
 *   2. @keyframe rules that Recharts uses to animate stroke-dashoffset — the
 *      default (un-animated) state leaves stroke-dashoffset at the full path
 *      length, so every line renders as invisible.
 *
 * Fix: inject a temporary <style> element inside the captured element that
 * re-declares the resolved CSS variables and freezes all animations at their
 * completed state. The element is removed after capture.
 */
export async function captureChartAsPng(
  element: HTMLElement,
  pixelRatio = 3,
): Promise<string> {
  // Resolve the chart CSS custom properties from the live document root.
  const rootStyle = getComputedStyle(document.documentElement);
  const varNames = [
    "--chart-1",
    "--chart-2",
    "--chart-3",
    "--chart-4",
    "--chart-5",
    "--background",
    "--foreground",
    "--muted",
    "--muted-foreground",
    "--border",
    "--card",
    "--card-foreground",
    "--primary",
    "--primary-foreground",
  ];
  const resolvedVars = varNames
    .map((v) => `${v}: ${rootStyle.getPropertyValue(v).trim()}`)
    .filter((entry) => !entry.endsWith(": "))
    .join("; ");

  const styleEl = document.createElement("style");
  styleEl.textContent = [
    // Make resolved vars available to the cloned document's :root.
    `:root { ${resolvedVars} }`,
    // Freeze Recharts stroke-dasharray animation so lines are fully drawn.
    `* { animation-duration: 0.001s !important; animation-delay: -1s !important; transition-duration: 0s !important; }`,
    `path { stroke-dashoffset: 0 !important; }`,
  ].join("\n");

  element.appendChild(styleEl);

  try {
    return await toPng(element, {
      cacheBust: true,
      pixelRatio,
      backgroundColor: "#ffffff",
    });
  } finally {
    element.removeChild(styleEl);
  }
}
