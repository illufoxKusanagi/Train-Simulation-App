# 📚 Quick Reference: Daily Reminder Integration Patterns

> **Purpose:** Copy-paste examples from your working daily-reminder project  
> **For:** Train Simulation App development

---

## 🏗️ Architecture Comparison

### Daily Reminder (Working ✅)
```
Qt Desktop App
├── QWebEngineView → loads http://localhost:3000 (dev)
├── QHttpServer (port 8080) → REST API
├── Database → SQLite for events
└── Frontend → Next.js + React (calendar UI)
```

### Train Simulation (What You're Building 🚧)
```
Qt Desktop App
├── QWebEngineView → loads http://localhost:3000 (dev)
├── QHttpServer (port 8080) → REST API
├── AppContext → In-memory data + simulation engine
└── Frontend → Next.js + React (parameters + charts)
```

**INSIGHT:** Same structure! Just swap SQLite → AppContext, Events → Train Data

---

## 🔄 API Pattern Comparison

### From Daily Reminder

#### Backend Route (httpserver.cpp)
```cpp
// GET all events
m_httpServer->route("/api/event", QHttpServerRequest::Method::Get,
    [this]() {
        return handleGetEvents();
    });

// POST new event
m_httpServer->route("/api/event", QHttpServerRequest::Method::Post,
    [this](const QHttpServerRequest &request) {
        QJsonObject data = parseRequestBody(request);
        return handlePostEvent(data);
    });
```

#### Frontend API Client (requests.ts)
```typescript
const API_BASE_URL = "http://localhost:8080";

export const getEvents = async (): Promise<IEvent[]> => {
  const response = await fetch(`${API_BASE_URL}/api/event`);
  if (!response.ok) throw new Error("Failed to fetch events");
  return response.json();
};

export const createEvent = async (event: IEvent): Promise<IEvent> => {
  const response = await fetch(`${API_BASE_URL}/api/event`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(event),
  });
  if (!response.ok) throw new Error("Failed to create event");
  return response.json();
};
```

### Apply to Train Simulation

#### Backend Route (http_server.cpp) - ✅ You Already Have This!
```cpp
// GET train parameters
m_httpServer->route("/api/parameters/train", QHttpServerRequest::Method::Get,
    [this, addCorsHeaders]() {
        return addCorsHeaders(m_apiHandler->handleGetTrainParameters());
    });

// POST train parameters
m_httpServer->route("/api/parameters/train", QHttpServerRequest::Method::Post,
    [this, addCorsHeaders](const QHttpServerRequest &request) {
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(m_apiHandler->handleUpdateTrainParameters(data));
    });
```

#### Frontend API Client - ⚠️ YOU NEED TO CREATE THIS!

Create: `frontend/src/services/api.ts`

```typescript
const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || "http://localhost:8080";

// Health check
export const checkHealth = async () => {
  const res = await fetch(`${API_BASE_URL}/api/health`);
  if (!res.ok) throw new Error("Backend not responding");
  return res.json();
};

// Train parameters
export const getTrainParameters = async () => {
  const res = await fetch(`${API_BASE_URL}/api/parameters/train`);
  if (!res.ok) throw new Error("Failed to get train parameters");
  return res.json();
};

export const updateTrainParameters = async (params: any) => {
  const res = await fetch(`${API_BASE_URL}/api/parameters/train`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ trainParameters: params }),
  });
  if (!res.ok) throw new Error("Failed to update train parameters");
  return res.json();
};

// Start simulation
export const startSimulation = async (params: any) => {
  const res = await fetch(`${API_BASE_URL}/api/simulation/start`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(params),
  });
  if (!res.ok) throw new Error("Failed to start simulation");
  return res.json();
};

// Get results
export const getSimulationResults = async () => {
  const res = await fetch(`${API_BASE_URL}/api/simulation/results`);
  if (!res.ok) throw new Error("Failed to get results");
  return res.json();
};

// Export wrapper
export const api = {
  checkHealth,
  getTrainParameters,
  updateTrainParameters,
  startSimulation,
  getSimulationResults,
};
```

---

## 🎨 React Component Patterns

### From Daily Reminder: Event Form

```tsx
// add-edit-event-dialog.tsx
"use client";

import { useState } from "react";
import { createEvent, updateEvent } from "../requests";

export function EventDialog() {
  const [formData, setFormData] = useState({
    title: "",
    category: "",
    startDate: "",
    // ... more fields
  });

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    
    try {
      if (isEditMode) {
        await updateEvent(eventId, formData);
      } else {
        await createEvent(formData);
      }
      
      // Refresh data
      onSuccess();
    } catch (error) {
      alert("Error: " + error.message);
    }
  };

  return (
    <form onSubmit={handleSubmit}>
      <input
        value={formData.title}
        onChange={(e) => setFormData({ ...formData, title: e.target.value })}
      />
      {/* More inputs */}
      <button type="submit">Save</button>
    </form>
  );
}
```

### Apply to Train Simulation: Parameter Form

Create: `frontend/src/components/forms/TrainParameterForm.tsx`

```tsx
"use client";

import { useState, useEffect } from "react";
import { api } from "@/services/api";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";

export function TrainParameterForm() {
  const [params, setParams] = useState({
    tractionMotors: 0,
    axles: 0,
    cars: 0,
    gearRatio: 0,
    wheelDiameter: 0,
  });
  const [isLoading, setIsLoading] = useState(false);

  // Load current parameters on mount
  useEffect(() => {
    loadParameters();
  }, []);

  const loadParameters = async () => {
    try {
      const data = await api.getTrainParameters();
      if (data.trainParameters) {
        setParams(data.trainParameters);
      }
    } catch (error) {
      console.error("Failed to load parameters:", error);
    }
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setIsLoading(true);

    try {
      const result = await api.updateTrainParameters(params);
      
      if (result.status === "success") {
        alert("Parameters updated successfully!");
      } else {
        alert("Error: " + result.message);
      }
    } catch (error) {
      alert("Failed to update: " + error.message);
    } finally {
      setIsLoading(false);
    }
  };

  const handleChange = (field: string, value: number) => {
    setParams({ ...params, [field]: value });
  };

  return (
    <form onSubmit={handleSubmit} className="space-y-4">
      <div>
        <Label htmlFor="tractionMotors">Traction Motors</Label>
        <Input
          id="tractionMotors"
          type="number"
          value={params.tractionMotors}
          onChange={(e) => handleChange("tractionMotors", parseFloat(e.target.value))}
          disabled={isLoading}
        />
      </div>

      <div>
        <Label htmlFor="axles">Number of Axles</Label>
        <Input
          id="axles"
          type="number"
          value={params.axles}
          onChange={(e) => handleChange("axles", parseFloat(e.target.value))}
          disabled={isLoading}
        />
      </div>

      <div>
        <Label htmlFor="cars">Number of Cars</Label>
        <Input
          id="cars"
          type="number"
          value={params.cars}
          onChange={(e) => handleChange("cars", parseFloat(e.target.value))}
          disabled={isLoading}
        />
      </div>

      <div>
        <Label htmlFor="gearRatio">Gear Ratio</Label>
        <Input
          id="gearRatio"
          type="number"
          step="0.01"
          value={params.gearRatio}
          onChange={(e) => handleChange("gearRatio", parseFloat(e.target.value))}
          disabled={isLoading}
        />
      </div>

      <div>
        <Label htmlFor="wheelDiameter">Wheel Diameter (mm)</Label>
        <Input
          id="wheelDiameter"
          type="number"
          value={params.wheelDiameter}
          onChange={(e) => handleChange("wheelDiameter", parseFloat(e.target.value))}
          disabled={isLoading}
        />
      </div>

      <Button type="submit" disabled={isLoading}>
        {isLoading ? "Saving..." : "Save Parameters"}
      </Button>
    </form>
  );
}
```

---

## 🚀 Simulation Control Pattern

### Create: `frontend/src/components/SimulationControl.tsx`

```tsx
"use client";

import { useState } from "react";
import { api } from "@/services/api";
import { Button } from "@/components/ui/button";
import { Play, Square, Loader2 } from "lucide-react";

export function SimulationControl() {
  const [isRunning, setIsRunning] = useState(false);
  const [results, setResults] = useState<any>(null);

  const handleStart = async () => {
    setIsRunning(true);
    setResults(null);

    try {
      // Start simulation
      const startResult = await api.startSimulation({
        // Include any necessary parameters
      });

      if (startResult.status === "success") {
        // Wait a bit for simulation to complete
        setTimeout(async () => {
          // Fetch results
          const resultsData = await api.getSimulationResults();
          setResults(resultsData);
          setIsRunning(false);
        }, 2000); // Adjust based on simulation time
      }
    } catch (error) {
      alert("Simulation failed: " + error.message);
      setIsRunning(false);
    }
  };

  return (
    <div className="space-y-4">
      <div className="flex gap-2">
        <Button
          onClick={handleStart}
          disabled={isRunning}
          className="flex items-center gap-2"
        >
          {isRunning ? (
            <>
              <Loader2 className="w-4 h-4 animate-spin" />
              Running...
            </>
          ) : (
            <>
              <Play className="w-4 h-4" />
              Start Simulation
            </>
          )}
        </Button>
      </div>

      {results && (
        <div className="border rounded p-4">
          <h3 className="font-semibold mb-2">Results:</h3>
          <pre className="text-sm">{JSON.stringify(results, null, 2)}</pre>
        </div>
      )}
    </div>
  );
}
```

---

## 🛠️ Build Scripts

### From Daily Reminder: dev.sh

```bash
#!/bin/bash

# Start frontend
cd frontend
npm run dev &
FRONTEND_PID=$!

# Start backend
cd ../backend/build
cmake .. && cmake --build .
./DailyReminder

# Kill frontend on exit
kill $FRONTEND_PID
```

### Your Train Simulation: build.sh (Already exists!)

```bash
#!/bin/bash

case "$1" in
    dev)
        # Development mode
        echo "Starting development mode..."
        cd frontend && npm run dev &
        cd backend/build && cmake .. && cmake --build . && ./bin/TrainSimulationApp
        ;;
    prod)
        # Production mode
        echo "Building for production..."
        cd frontend && npm run build
        cd ../backend/build
        cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
        ;;
    *)
        echo "Usage: $0 {dev|prod}"
        exit 1
        ;;
esac
```

---

## 📱 Frontend Page Structure

### From Daily Reminder: Main Page

```
frontend/src/
├── app/
│   ├── page.tsx              # Main dashboard
│   ├── layout.tsx            # Root layout
│   └── globals.css
├── modules/
│   └── calendar/
│       ├── components/       # UI components
│       ├── dialogs/          # Forms
│       ├── contexts/         # State management
│       ├── hooks/            # Custom hooks
│       ├── interfaces.ts     # TypeScript types
│       └── requests.ts       # API calls
```

### Your Train Simulation: Recommended Structure

```
frontend/src/
├── app/
│   ├── page.tsx                    # Dashboard
│   ├── layout.tsx
│   ├── train-parameter/
│   │   └── page.tsx               # Train params page
│   ├── electrical-parameter/
│   │   └── page.tsx               # Electrical params page
│   ├── simulation/
│   │   └── page.tsx               # Simulation control
│   └── results/
│       └── page.tsx               # Results & charts
├── components/
│   ├── forms/
│   │   ├── TrainParameterForm.tsx
│   │   ├── ElectricalParameterForm.tsx
│   │   └── ...
│   ├── simulation/
│   │   └── SimulationControl.tsx
│   └── charts/
│       ├── SpeedChart.tsx
│       └── PowerChart.tsx
├── services/
│   └── api.ts                     # API client
└── types/
    └── index.ts                   # TypeScript interfaces
```

---

## 🎯 TypeScript Types

### Create: `frontend/src/types/index.ts`

```typescript
export interface TrainParameters {
  tractionMotors: number;
  axles: number;
  cars: number;
  gearRatio: number;
  wheelDiameter: number;
  trainsetLength: number;
  motorCars1: number;
  motorCars2: number;
  trailerCabCars: number;
}

export interface ElectricalParameters {
  wheelPower: number;
  motorOutPower: number;
  motorInPower: number;
  vvvfInPower: number;
  catenaryPower: number;
  apsPower: number;
  staticApsPower: number;
}

export interface SimulationResults {
  speed: number[];
  distance: number[];
  power: number[];
  energy: number[];
  time: number[];
}

export interface ApiResponse<T = any> {
  status: "success" | "error";
  message?: string;
  data?: T;
}
```

---

## ✅ Development Checklist

### Phase 1: API Client Setup
- [ ] Create `frontend/src/services/api.ts`
- [ ] Add all API functions (health, parameters, simulation)
- [ ] Test each function with `curl` first
- [ ] Add error handling

### Phase 2: Forms
- [ ] Create `TrainParameterForm.tsx`
- [ ] Create `ElectricalParameterForm.tsx`
- [ ] Create `RunningParameterForm.tsx`
- [ ] Create `TrackParameterForm.tsx`
- [ ] Add form validation
- [ ] Add loading states

### Phase 3: Simulation
- [ ] Create `SimulationControl.tsx`
- [ ] Add start/stop buttons
- [ ] Show loading indicator
- [ ] Fetch and display results

### Phase 4: Visualization
- [ ] Install chart library (`npm install recharts`)
- [ ] Create `SpeedChart.tsx`
- [ ] Create `PowerChart.tsx`
- [ ] Create `EnergyChart.tsx`
- [ ] Add chart controls

### Phase 5: Testing
- [ ] Test all forms
- [ ] Test simulation trigger
- [ ] Test results display
- [ ] Test in QWebEngineView
- [ ] Test production build

---

## 🚨 Common Mistakes to Avoid

### ❌ DON'T:
```typescript
// Hardcoded API URL
const response = await fetch('http://localhost:8080/api/...');
```

### ✅ DO:
```typescript
// Use environment variable
const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8080';
const response = await fetch(`${API_BASE_URL}/api/...`);
```

---

### ❌ DON'T:
```tsx
// Forgetting error handling
const data = await api.getTrainParameters();
setParams(data);
```

### ✅ DO:
```tsx
// Always handle errors
try {
  const data = await api.getTrainParameters();
  setParams(data.trainParameters);
} catch (error) {
  console.error('Failed to load:', error);
  alert('Failed to load parameters');
}
```

---

### ❌ DON'T:
```cpp
// Missing CORS headers
return QHttpServerResponse(jsonData);
```

### ✅ DO:
```cpp
// Always add CORS headers
return addCorsHeaders(QHttpServerResponse(jsonData));
```

---

## 🔗 Quick Links

- **Daily Reminder Repo:** `/path/to/daily-reminder`
- **API Test Script:** `./scripts/test-api.sh`
- **Build Script:** `./build.sh dev`
- **Qt Docs:** https://doc.qt.io/qt-6/
- **Next.js Docs:** https://nextjs.org/docs

---

## 💡 Pro Tips

1. **Always test backend first:** Use `curl` before testing frontend
2. **Use daily-reminder as reference:** It's your working example!
3. **Copy-paste is okay:** Adapt working code from daily-reminder
4. **Small increments:** One endpoint → one form → test → repeat
5. **Git commit often:** Every working feature should be committed

---

**Remember:** You already proved you can do this with daily-reminder. Just apply the same patterns! 🚀
