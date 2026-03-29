"use client";

import { NextIntlClientProvider } from "next-intl";
import { useEffect, useState } from "react";

import enMessages from "../../messages/en.json";

type Messages = Record<string, unknown>;

export function LocaleProvider({ children }: { children: React.ReactNode }) {
  const [locale, setLocale] = useState("en");
  const [messages, setMessages] = useState<Messages>(enMessages);

  useEffect(() => {
    const saved = localStorage.getItem("locale");
    if (saved && saved !== "en") {
      setLocale(saved);
      import(`../../messages/${saved}.json`).then((m) =>
        setMessages(m.default as Messages),
      );
    }
  }, []);

  return (
    <NextIntlClientProvider locale={locale} messages={messages}>
      {children}
    </NextIntlClientProvider>
  );
}
