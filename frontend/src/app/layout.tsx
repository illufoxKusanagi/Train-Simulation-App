import type { Metadata } from "next";
import { Roboto, Geist_Mono } from "next/font/google";
import "./globals.css";
import { ThemeProvider } from "next-themes";
import { Toaster } from "sonner";
import { FormPersistenceProvider } from "@/contexts/FormPersistenceContext";
import { AuthProvider } from "@/contexts/AuthContext";

const roboto = Roboto({
  variable: "--font-roboto",
  subsets: ["latin"],
  weight: ["400", "600"],
});

const geistMono = Geist_Mono({
  variable: "--font-geist-mono",
  subsets: ["latin"],
});

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
      <body className={`${roboto.variable} ${geistMono.variable} antialiased`}>
        <FormPersistenceProvider>
          <ThemeProvider attribute="class" defaultTheme="system" enableSystem>
            <AuthProvider>
              {children}
            </AuthProvider>
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
