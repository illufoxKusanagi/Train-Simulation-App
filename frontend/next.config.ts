import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  // Remove 'output: export' for standard web deployment
  // Keep it only if you need static HTML export
  // output: "export", // Generate static HTML export

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
