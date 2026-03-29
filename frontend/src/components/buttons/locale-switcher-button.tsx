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
  const [current, setCurrent] = useState("en");

  useEffect(() => {
    const saved = localStorage.getItem("locale") ?? "en";
    setCurrent(saved);
  }, []);

  const switchLocale = (locale: string) => {
    localStorage.setItem("locale", locale);
    window.location.reload();
  };

  const display = LOCALE_MAP[current] ?? LOCALE_MAP.en;

  return (
    <div>
      <DropdownMenu>
        <DropdownMenuTrigger asChild>
          <Button variant={"outline"} size={"sm"}>
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
