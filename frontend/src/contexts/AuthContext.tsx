"use client";

import React, {
  createContext,
  useContext,
  useState,
  useEffect,
  useCallback,
} from "react";
import { useRouter, usePathname } from "next/navigation";
import { api } from "@/services/api";

interface AuthContextType {
  isAuthenticated: boolean;
  isChecking: boolean;
  logout: () => void;
}

const AuthContext = createContext<AuthContextType | undefined>(undefined);

// Pages that do NOT require authentication
const PUBLIC_PATHS = ["/"];

export function AuthProvider({ children }: { children: React.ReactNode }) {
  const [isAuthenticated, setIsAuthenticated] = useState(false);
  const [isChecking, setIsChecking] = useState(true);
  const router = useRouter();
  const pathname = usePathname();

  const checkAuth = useCallback(async () => {
    try {
      const { isAuthenticated: authed } = await api.getAuthStatus();
      setIsAuthenticated(authed);
      if (!authed && !PUBLIC_PATHS.includes(pathname)) {
        router.replace("/");
      }
    } catch {
      // Backend not reachable — treat as unauthenticated
      setIsAuthenticated(false);
      if (!PUBLIC_PATHS.includes(pathname)) {
        router.replace("/");
      }
    } finally {
      setIsChecking(false);
    }
  }, [pathname, router]);

  useEffect(() => {
    checkAuth();
  }, [checkAuth]);

  const logout = useCallback(() => {
    setIsAuthenticated(false);
    router.replace("/");
  }, [router]);

  return (
    <AuthContext.Provider value={{ isAuthenticated, isChecking, logout }}>
      {children}
    </AuthContext.Provider>
  );
}

export function useAuth() {
  const ctx = useContext(AuthContext);
  if (!ctx) throw new Error("useAuth must be used within AuthProvider");
  return ctx;
}
