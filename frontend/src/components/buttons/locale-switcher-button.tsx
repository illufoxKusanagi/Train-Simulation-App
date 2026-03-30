"use client";

import { useEffect, useState } from "react";
import { Button } from "../ui/button";
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuTrigger,
} from "../ui/dropdown-menu";

const LOCALE_MAP: Record<string, { flag: string; label: string }> = {
  en: { flag: "🇬🇧", label: "EN" },
  id: { flag: "🇮🇩", label: "ID" },
};

export function LocaleSwitcherButton() {
  type Locale = keyof typeof LOCALE_MAP;
  const [current, setCurrent] = useState<Locale>("en");

  useEffect(() => {
    const saved = localStorage.getItem("locale");
    if (saved === "en" || saved === "id") setCurrent(saved);
  }, []);

  const switchLocale = (locale: Locale) => {
    localStorage.setItem("locale", locale);
    window.location.reload();
  };

  const display = LOCALE_MAP[current] ?? LOCALE_MAP.en;
  return (
    <div>
      <DropdownMenu>
        <DropdownMenuTrigger asChild>
          <Button variant={"outline"} size={"default"}>
            {display.flag} {display.label}
          </Button>
        </DropdownMenuTrigger>
        <DropdownMenuContent>
          <DropdownMenuItem onClick={() => switchLocale("en")}>
            🇬🇧 English
          </DropdownMenuItem>
          <DropdownMenuItem onClick={() => switchLocale("id")}>
            🇮🇩 Bahasa Indonesia
          </DropdownMenuItem>
        </DropdownMenuContent>
      </DropdownMenu>
    </div>
  );
}
