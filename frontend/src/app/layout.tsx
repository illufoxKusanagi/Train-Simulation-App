import type { Metadata } from "next";
import "./globals.css";
import { ThemeProvider } from "next-themes";
import { Toaster } from "sonner";
import { FormPersistenceProvider } from "@/contexts/FormPersistenceContext";

export const metadata: Metadata = {
  title: "Train Simulation App",
  description: "Train simulation desktop app",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" suppressHydrationWarning>
      <body className="antialiased">
        <FormPersistenceProvider>
          <ThemeProvider attribute="class" defaultTheme="system" enableSystem>
            {children}
          </ThemeProvider>
          <Toaster
            position="top-center"
            richColors
            closeButton
            duration={2000}
          />
        </FormPersistenceProvider>
      </body>
    </html>
  );
}
