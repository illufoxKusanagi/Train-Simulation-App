"use client";

import { NextIntlClientProvider } from "next-intl";
import { useCallback, useEffect, useState } from "react";

import enMessages from "../../messages/en.json";

type Messages = Record<string, unknown>;

const SUPPORTED_LOCALES = ["en", "id"];

export function LocaleProvider({ children }: { children: React.ReactNode }) {
  const [locale, setLocale] = useState("en");
  const [messages, setMessages] = useState<Messages>(enMessages);

  const loadLocale = useCallback(async (target: string) => {
    if (target === "en") {
      setMessages(enMessages);
      setLocale("en");
      return;
    }
    if (!SUPPORTED_LOCALES.includes(target)) return;
    try {
      const m = await import(`../../messages/${target}.json`);
      setMessages(m.default as Messages);
      setLocale(target);
    } catch (err) {
      console.error(`Failed to load locale "${target}":`, err);
    }
  }, []);

  useEffect(() => {
    const saved = localStorage.getItem("locale");
    if (saved && saved !== "en" && SUPPORTED_LOCALES.includes(saved)) {
      loadLocale(saved);
    }
  }, [loadLocale]);

  // Listen for locale-change events dispatched by the locale switcher button
  useEffect(() => {
    const handler = (e: Event) => {
      const detail = (e as CustomEvent<string>).detail;
      if (detail) {
        loadLocale(detail);
      }
    };
    window.addEventListener("locale-change", handler);
    return () => window.removeEventListener("locale-change", handler);
  }, [loadLocale]);

  return (
    <NextIntlClientProvider locale={locale} messages={messages}>
      {children}
    </NextIntlClientProvider>
  );
}
