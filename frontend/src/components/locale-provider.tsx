"use client";

import { NextIntlClientProvider } from "next-intl";
import { useEffect, useState } from "react";

import enMessages from "../../messages/en.json";

type Messages = Record<string, unknown>;

const SUPPORTED_LOCALES = ["en", "id"];

export function LocaleProvider({ children }: { children: React.ReactNode }) {
  const [locale, setLocale] = useState("en");
  const [messages, setMessages] = useState<Messages>(enMessages);

  useEffect(() => {
    const saved = localStorage.getItem("locale");
    if (saved && saved !== "en" && SUPPORTED_LOCALES.includes(saved)) {
      setLocale(saved);
      import(`../../messages/${saved}.json`)
        .then((m) => setMessages(m.default as Messages))
        .catch((err) => {
          console.error(`Failed to load locale "${saved}":`, err);
        });
    }
  }, []);

  return (
    <NextIntlClientProvider locale={locale} messages={messages}>
      {children}
    </NextIntlClientProvider>
  );
}
