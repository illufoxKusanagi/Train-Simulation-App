"use client";

import React, { createContext, useContext, useCallback } from "react";

interface FormPersistenceContextType {
  saveFormData: (formKey: string, data: Record<string, unknown>) => void;
  loadFormData: (formKey: string) => Record<string, unknown> | null;
  clearFormData: (formKey: string) => void;
  clearAllFormData: () => void;
}

const FormPersistenceContext = createContext<
  FormPersistenceContextType | undefined
>(undefined);

export function FormPersistenceProvider({
  children,
}: {
  children: React.ReactNode;
}) {
  const saveFormData = useCallback(
    (formKey: string, data: Record<string, unknown>) => {
      if (typeof window === "undefined") return;
      try {
        localStorage.setItem(`form_${formKey}`, JSON.stringify(data));
      } catch (error) {
        console.error(`Failed to save form data for ${formKey}:`, error);
      }
    },
    []
  );

  const loadFormData = useCallback((formKey: string) => {
    if (typeof window === "undefined") return null;
    try {
      const saved = localStorage.getItem(`form_${formKey}`);
      return saved ? JSON.parse(saved) : null;
    } catch (error) {
      console.error(`Failed to load form data for ${formKey}:`, error);
      return null;
    }
  }, []);

  const clearFormData = useCallback((formKey: string) => {
    if (typeof window === "undefined") return;
    try {
      localStorage.removeItem(`form_${formKey}`);
    } catch (error) {
      console.error(`Failed to clear form data for ${formKey}:`, error);
    }
  }, []);

  const clearAllFormData = useCallback(() => {
    if (typeof window === "undefined") return;
    try {
      const keys = Object.keys(localStorage);
      keys.forEach((key) => {
        if (key.startsWith("form_")) {
          localStorage.removeItem(key);
        }
      });
    } catch (error) {
      console.error("Failed to clear all form data:", error);
    }
  }, []);

  return (
    <FormPersistenceContext.Provider
      value={{ saveFormData, loadFormData, clearFormData, clearAllFormData }}
    >
      {children}
    </FormPersistenceContext.Provider>
  );
}

export function useFormPersistence() {
  const context = useContext(FormPersistenceContext);
  if (!context) {
    throw new Error(
      "useFormPersistence must be used within FormPersistenceProvider"
    );
  }
  return context;
}
