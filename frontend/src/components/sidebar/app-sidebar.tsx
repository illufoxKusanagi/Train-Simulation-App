"use client";
import {
  Activity,
  BarChart3,
  Calendar,
  Home,
  Rocket,
  Settings,
  Ticket,
  TrainFront,
  Waypoints,
  Zap,
} from "lucide-react";

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
import Image from "next/image";
import { usePathname } from "next/navigation";

export function AppSidebar() {
  const { open } = useSidebar();
  const pathname = usePathname();

  const configItems = [
    {
      title: "Train Parameter",
      url: "/train-parameter",
      icon: TrainFront,
    },
    {
      title: "Track Parameter",
      url: "/track-parameter",
      icon: Waypoints,
    },
    {
      title: "Running Parameter",
      url: "/running-parameter",
      icon: Activity,
    },
    {
      title: "Electrical Parameter",
      url: "/electrical-parameter",
      icon: Zap,
    },
  ];

  const analysisItems = [
    {
      title: "Output",
      url: "/output",
      icon: BarChart3,
    },
    {
      title: "Optimization",
      url: "/optimization",
      icon: Rocket,
    },
  ];

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
        {open ? (
          <div className="flex flex-row gap-2 items-center w-full mt-10 mb-3">
            <Image src="/logo.png" alt="Logo" width={50} height={50} />
            <div className="flex flex-col">
              <p className="font-bold text-lg text-primary leading-none">
                Train Simulation App
              </p>
            </div>
          </div>
        ) : (
          <div className="flex items-center justify-center w-full mt-11">
            <Image src="/logo.png" alt="Logo" width={16} height={16} />
          </div>
        )}
      </SidebarHeader>
      <SidebarContent className="flex flex-col gap-4 transition-all duration-500 ease-in-out">
        <SidebarGroup className={cn(open ? "" : "mt-6")}>
          <SidebarGroupContent>
            <SidebarGroupLabel>Input Menus</SidebarGroupLabel>
            <SidebarMenu>
              {configItems.map((item) => (
                <SidebarHeader key={item.title}>
                  <SidebarMenuButton
                    asChild
                    key={item.title}
                    isActive={pathname === item.url}
                    tooltip={item.title}
                  >
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
        <SidebarGroup>
          <SidebarGroupContent>
            <SidebarGroupLabel>Output Menus</SidebarGroupLabel>
            <SidebarMenu>
              {analysisItems.map((item) => (
                <SidebarHeader key={item.title}>
                  <SidebarMenuButton
                    asChild
                    key={item.title}
                    isActive={pathname === item.url}
                    tooltip={item.title}
                  >
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
      </SidebarContent>
      <SidebarFooter
        className={cn(
          "w-full bg-accent/50 backdrop-blur supports-[backdrop-filter]:bg-background/60 transition-all duration-300",
          open ? "p-4" : "p-2"
        )}
      >
        {open ? (
          <p className="body-small-bold text-center">
            Made with ❤️ by{" "}
            <Link href={"https://github.com/illufoxKusanagi"}>
              <span className="hover:underline text-primary-600 dark:text-primary-300">
                illufoxKusanagi
              </span>
            </Link>
          </p>
        ) : (
          <div className="flex items-center justify-center h-6">
            <Link href={"https://github.com/illufoxKusanagi"}>
              <span className="text-xl">❤️</span>
            </Link>
          </div>
        )}
      </SidebarFooter>
    </Sidebar>
  );
}
