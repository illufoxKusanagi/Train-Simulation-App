"use client";
import { Calendar, Home, Network, Settings, Ticket } from "lucide-react";

import {
  Sidebar,
  SidebarContent,
  SidebarFooter,
  SidebarGroup,
  SidebarGroupContent,
  SidebarGroupLabel,
  SidebarHeader,
  SidebarMenu,
  SidebarMenuButton,
  SidebarTrigger,
  useSidebar,
} from "@/components/ui/sidebar";
import { cn } from "@/lib/utils";
import Link from "next/link";
import { CardDescription } from "../ui/card";

export function AppSidebar() {
  const { open } = useSidebar();

  const user = {
    name: "arief",
    email: "ariefsatria@gmail.com",
    avatar: "",
  };

  const items = [
    {
      title: "Dashboard",
      url: "/",
      icon: Home,
    },
    {
      title: "Train Parameter",
      url: "/train-parameter",
      icon: Ticket,
    },
    {
      title: "Track Parameter",
      url: "/track-parameter",
      icon: Calendar,
    },
    {
      title: "Running Parameter",
      url: "/running-parameter",
      icon: Calendar,
    },
    {
      title: "Electrical Parameter",
      url: "/electrical-parameter",
      icon: Calendar,
    },
    {
      title: "Output",
      url: "/output",
      icon: Settings,
    },
  ];
  return (
    <Sidebar variant="floating" collapsible="icon" className="z-50">
      <SidebarTrigger
        size={"lg"}
        className={cn("absolute", open ? "top-6 left-6" : "top-4 left-4")}
      />
      <SidebarHeader
        className={cn(
          "overflow-hidden transition-all duration-500 ease-in-out",
          open
            ? "px-4 pt-4 block opacity-100"
            : "px-2 pt-4 flex items-center justify-center opacity-100"
        )}
      >
        <SidebarMenu>
          <SidebarMenuButton asChild className="hover:bg-accent/50 h-full">
            {open ? (
              <div className="flex items-center gap-3 w-full px-2 mt-16">
                <Network className="h-6 w-6 text-primary-600 dark:text-primary-300 shrink-0" />
                <div className="flex flex-col items-start overflow-hidden">
                  <p className="body-big-bold text-primary-600 dark:text-primary-300 truncate">
                    Train Simulation App
                  </p>
                </div>
              </div>
            ) : (
              <div className="flex items-center justify-center w-full mt-11">
                <Network className="h-6 w-6 text-primary-600 dark:text-primary-300" />
              </div>
            )}
          </SidebarMenuButton>
        </SidebarMenu>
      </SidebarHeader>
      <SidebarContent className="flex flex-col gap-4 transition-all duration-500 ease-in-out">
        <SidebarMenu>
          <SidebarGroup>
            <SidebarGroupContent className="space-y-1">
              <SidebarGroupLabel className={cn(open ? "block" : "hidden")}>
                Input Menus
              </SidebarGroupLabel>
              <SidebarMenu>
                {items.map((item) => (
                  <SidebarHeader key={item.title}>
                    <SidebarMenuButton asChild>
                      <Link href={item.url}>
                        <item.icon />
                        <span>{item.title}</span>
                      </Link>
                    </SidebarMenuButton>
                  </SidebarHeader>
                ))}
              </SidebarMenu>
            </SidebarGroupContent>
          </SidebarGroup>
        </SidebarMenu>
      </SidebarContent>
      <SidebarFooter
        className={cn(
          "absolute bottom-0 w-full border-t bg-background/95 backdrop-blur supports-[backdrop-filter]:bg-background/60",
          open ? "p-0" : "p-0"
        )}
      ></SidebarFooter>
    </Sidebar>
  );
}
