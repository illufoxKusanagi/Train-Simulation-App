import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  // Remove 'output: export' for standard web deployment
  // Keep it only if you need static HTML export
  output: "export", // Generate static HTML export

  // Required for static export to work with local file system
  images: {
    unoptimized: true,
  },
  // Use relative paths for assets so they load correctly via file:// protocol
  assetPrefix: ".",

  // Rewrites don't work in static export, but we keep this for dev mode
  async rewrites() {
    return [
      {
        source: "/api/:path*",
        destination:
          process.env.NEXT_PUBLIC_API_URL || "http://localhost:8080/api/:path*",
      },
    ];
  },

  // Environment variables
  env: {
    NEXT_PUBLIC_API_URL:
      process.env.NEXT_PUBLIC_API_URL || "http://localhost:8080",
  },
};

export default nextConfig;
