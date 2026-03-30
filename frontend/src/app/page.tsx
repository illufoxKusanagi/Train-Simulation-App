"use client";

import { useState } from "react";
import { useRouter } from "next/navigation";
import { api } from "@/services/api";
import { Button } from "@/components/ui/button";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
  CardFooter,
} from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Lock, User, Loader2, Eye, EyeOff } from "lucide-react";
import { toast } from "sonner";
import { useTranslations } from "next-intl";
import { ModeToggle } from "@/components/toggle-mode-button";
import { LocaleSwitcherButton } from "@/components/buttons/locale-switcher-button";
import Image from "next/image";
import { openUrlInBrowser } from "@/lib/qt-webchannel";

export default function Home() {
  const router = useRouter();
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [showPassword, setShowPassword] = useState(false);
  const [loading, setLoading] = useState(false);
  const t = useTranslations("Login");

  const handleLogin = async (e: React.FormEvent) => {
    e.preventDefault();

    if (!username || !password) {
      toast.error(t("toast.emptyFields"));
      return;
    }

    try {
      setLoading(true);
      const res = await api.login(username, password);

      if (res.status === "success") {
        // Initialize backend defaults after a successful login
        await api.quickInit().catch(() => {
          // quickInit failure is non-fatal — simulation just uses backend defaults
          console.warn("quickInit skipped or failed, continuing...");
        });
        toast.success(t("toast.success"));
        router.push("/train-parameter");
      }
    } catch (err: unknown) {
      const message = err instanceof Error ? err.message : t("toast.failed");
      toast.error(message);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="min-h-screen flex items-center justify-center bg-gradient-to-br from-blue-50 via-indigo-50 to-purple-50 dark:from-blue-950/20 dark:via-indigo-950/20 dark:to-purple-950/20 p-4 overflow-hidden relative">
      <div className="absolute right-4 top-4 flex flex-row gap-4">
        <LocaleSwitcherButton />
        <ModeToggle />
      </div>
      <div className="w-full max-w-md relative z-10">
        <div className="text-center mb-8">
          <div className="inline-flex items-center justify-center shadow-lg">
            <Image src="/logo.png" alt="Logo" width={64} height={64} />
          </div>
          <p className="heading-3 text-gray-900 dark:text-gray-100">
            {t("appTitle")}
          </p>
          <p className="text-muted-foreground body-medium-regular">
            {t("appSubtitle")}
          </p>
        </div>

        <Card className="border-0 shadow-2xl bg-white/80 dark:bg-gray-900/80 backdrop-blur-xl">
          <CardHeader className="space-y-1">
            <CardTitle className="text-2xl text-center">
              {t("cardTitle")}
            </CardTitle>
            <CardDescription className="text-center">
              {t("cardDescription")}
            </CardDescription>
          </CardHeader>
          <CardContent>
            <form onSubmit={handleLogin} className="space-y-4">
              <div className="space-y-2">
                <Label htmlFor="username">{t("usernameLabel")}</Label>
                <div className="relative">
                  <User className="absolute left-3 top-2.5 h-5 w-5 text-muted-foreground" />
                  <Input
                    id="username"
                    placeholder={t("usernamePlaceholder")}
                    type="text"
                    className="pl-10"
                    value={username}
                    onChange={(e) => setUsername(e.target.value)}
                    disabled={loading}
                  />
                </div>
              </div>
              <div className="space-y-2">
                <Label htmlFor="password">{t("passwordLabel")}</Label>
                <div className="relative">
                  <Lock className="absolute left-3 top-2.5 h-5 w-5 text-muted-foreground" />
                  <Input
                    id="password"
                    type={showPassword ? "text" : "password"}
                    placeholder="••••••••"
                    className="pl-10 pr-10"
                    value={password}
                    onChange={(e) => setPassword(e.target.value)}
                    disabled={loading}
                  />
                  <button
                    type="button"
                    onClick={() => setShowPassword(!showPassword)}
                    tabIndex={-1}
                    className="absolute right-3 top-2.5 text-muted-foreground hover:text-foreground transition-colors"
                  >
                    {showPassword ? (
                      <EyeOff className="h-5 w-5" />
                    ) : (
                      <Eye className="h-5 w-5" />
                    )}
                  </button>
                </div>
              </div>
              <Button
                type="submit"
                variant={"default"}
                className="w-full"
                disabled={loading}
              >
                {loading ? (
                  <>
                    <Loader2 className="mr-2 h-5 w-5 animate-spin" />
                    {t("loadingButton")}
                  </>
                ) : (
                  t("submitButton")
                )}
              </Button>
            </form>
          </CardContent>
          <CardFooter className="flex justify-center border-t">
            <p className="body-small-regular text-muted-foreground text-center">
              Developed with ❤️ by{" "}
              <span>
                <button
                  onClick={() => openUrlInBrowser("https://github.com/illufoxKusanagi")}
                  className="hover:underline focus:outline-none cursor-pointer"
                  type="button"
                >
                  Illufox Kusanagi
                </button>
              </span>{" "}
              <br />
              {t("footer")}
            </p>
          </CardFooter>
        </Card>
      </div>
    </div>
  );
}
