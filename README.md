# Train Simulation App

[![License: MIT](https://img.shields.io/badge/License-MIT-red.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/v/release/illufoxKusanagi/Train-Simulation-App?display_name=tag&sort=semver)](https://github.com/illufoxKusanagi/Train-Simulation-App/releases)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-blue)](#)
[![Qt](https://img.shields.io/badge/Qt-6.x-green)](https://www.qt.io/)
[![Next.js](https://img.shields.io/badge/Next.js-15+-black)](https://nextjs.org/)
[![TypeScript](https://img.shields.io/badge/TypeScript-5-blue)](https://www.typescriptlang.org/)
[![C++17](https://img.shields.io/badge/C++-17-orange)](https://isocpp.org/)

A desktop train simulation application for academic research — specifically a skripsi (undergraduate thesis) on **optimizing electric train specifications using fuzzy logic**. It simulates train physics, power and energy consumption, and applies Mamdani fuzzy logic to score and rank operating parameter combinations.

![trainSimulationAppLogo](https://github.com/user-attachments/assets/c8c9747c-b962-458f-bf7a-166f7b57ff48)

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Features](#features)
- [Tech Stack](#tech-stack)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [API Reference](#api-reference)
- [Simulation Engine](#simulation-engine)
- [Fuzzy Optimization](#fuzzy-optimization)
- [Contributing](#contributing)

---

## Architecture Overview

The app was originally a **pure Qt Widgets** desktop application (QWidgets + QChart). It has since been fully migrated to a **hybrid desktop architecture**:

```
┌─────────────────────────────────────────────────────────┐
│                Qt Desktop Window (C++)                  │
│                                                         │
│  ┌──────────────────────────────────────────────────┐   │
│  │  QWebEngineView  (Chromium rendering engine)     │   │
│  │                                                  │   │
│  │        Next.js 15 + React + Tailwind CSS         │   │
│  │        All UI, forms, charts, exports            │   │
│  │        http://localhost:3254 (dev)               │   │
│  │        file:///…/out/index.html (prod)           │   │
│  └──────────────────────────────────────────────────┘   │
│                      ⇅ HTTP REST                        │
│                   localhost:8080/api/*                   │
│  ┌──────────────────────────────────────────────────┐   │
│  │  Qt C++ Backend                                  │   │
│  │  HttpServer → ApiHandler → Controllers           │   │
│  │  Physics engine, fuzzy logic, AppContext         │   │
│  └──────────────────────────────────────────────────┘   │
│                   ⇅ Qt WebChannel                       │
│  ┌──────────────────────────────────────────────────┐   │
│  │  FileBridge (native file save dialogs)           │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

**What changed from the old pure-Qt version:**

| Aspect    | Old (Qt Widgets)                | New (Qt + Next.js)                             |
| --------- | ------------------------------- | ---------------------------------------------- |
| UI        | QWidgets + `.ui` files + QChart | React + Tailwind CSS + Recharts                |
| Forms     | Qt Designer forms               | React Hook Form + Zod validation               |
| Charts    | QChart                          | Recharts with custom styling                   |
| Exports   | Qt file I/O                     | Native dialog via Qt WebChannel + XLSX library |
| Styling   | Qt stylesheets (`.qss`)         | Tailwind CSS v4 + shadcn/ui                    |
| Dark mode | Manual                          | `next-themes` + CSS variables                  |
| Auth      | Qt login dialog                 | Removed                                        |
| Build     | qmake / Qt Creator only         | CMake (backend) + npm (frontend)               |

---

## Features

### Parameter Configuration

- **Train Parameters** — Car composition (M1/M2/Tc/T counts, up to 12-car trains), car masses, passenger counts, gear ratio, wheel diameter, axle count, inertia coefficients. Car-count presets auto-fill formation.
- **Running Parameters** — Start resistance, acceleration/deceleration rates, speed notch points (P1, P2, B1, B2), coasting delta, emergency braking rate.
- **Track Parameters** — Station count and spacing, gradient, curve radius, speed limit, dwell time. Supports CSV upload for slope/radius profiles.
- **Electrical Parameters** — Line voltage, motor voltage, power factor, gear/motor/VVVF efficiencies, APS power. Supports CSV upload for torque-speed curves.

All forms persist state in `localStorage` via `FormPersistenceContext` so inputs survive page navigation and app restarts.

### Simulation Engine

- **Static simulation** — Physics-based time-step integration across a full inter-station trip (powering → coasting → braking).
- **Dynamic simulation** — Adaptive simulation accounting for actual track geometry (slope, curve) at each step.
- Real-time status polling so the UI updates as the backend computes.

### Output & Visualization

Six dedicated chart tabs, each with independent CSV and Excel export:

| Tab         | Key variables                                                  |
| ----------- | -------------------------------------------------------------- |
| Speed       | Train speed, speed limit, acceleration                         |
| Power       | Wheel power, motor power in/out, VVVF power, catenary power    |
| Current     | VVVF RMS current, catenary current                             |
| Force       | Motor force, resistance force, tractive effort per motor       |
| Distance    | Distance per phase, total distance, odometer, braking distance |
| Power/Motor | Per-traction-motor power output                                |

Summary cards display: max speed, total distance, max traction effort, adhesion, peak catenary power/current, peak VVVF power/current, energy consumption, energy powering, max motor power per motor.

### Fuzzy Logic Optimization

Parameter sweep over user-defined ranges of acceleration (`acc_start_si`) and weakening point (`v_p1`). Each combination runs a full simulation and is scored by a **Mamdani fuzzy inference system**:

- Inputs: Acceleration, Weakening Point, Gradient, Speed Limit
- Membership functions: Triangle and Trapezoid sets
- Rules: IF-THEN knowledge base per thesis specification
- Defuzzification: Centroid method
- Output: Suitability score (0–100)

Live progress bar and best-result card update in real time during the sweep.

### Native File Dialogs

When running inside the Qt window, CSV and Excel exports use a native OS file-save dialog via Qt WebChannel (`FileBridge`). When accessed directly in a browser (dev mode), a standard browser download is used as fallback.

### Dark Mode

Full dark mode toggle (system / light / dark) via `next-themes`. All colors use CSS custom properties so switching is instantaneous.

---

## Tech Stack

### Frontend

| Library            | Version | Role                                            |
| ------------------ | ------- | ----------------------------------------------- |
| Next.js            | 15      | App Router, SSG (`output: export` for prod)     |
| React              | 19      | UI                                              |
| TypeScript         | 5       | Type safety                                     |
| Tailwind CSS       | 4       | Utility styling (CSS-first config via `@theme`) |
| shadcn/ui          | latest  | Component library (Radix UI primitives)         |
| Recharts           | 2       | Charts                                          |
| React Hook Form    | 7       | Form state                                      |
| Zod                | 4       | Schema validation                               |
| XLSX               | 0.18    | Excel export                                    |
| tailwind-scrollbar | 4       | Custom scrollbar styling                        |
| next-themes        | 0.4     | Dark mode                                       |
| sonner             | 2       | Toast notifications                             |

### Backend

| Technology    | Role                                              |
| ------------- | ------------------------------------------------- |
| C++17         | Language                                          |
| Qt 6          | Framework                                         |
| Qt HttpServer | REST API server on port 8080                      |
| Qt WebEngine  | Chromium container for the Next.js UI             |
| Qt WebChannel | Bridge for native file save dialogs               |
| Qt Concurrent | Async simulation/optimization on separate threads |
| CMake + Ninja | Build system                                      |

---

## Project Structure

```
train-app-simulation/
├── frontend/                    # Next.js application
│   └── src/
│       ├── app/
│       │   ├── page.tsx                   # Home / dashboard
│       │   ├── train-parameter/           # Train parameter form
│       │   ├── running-parameter/         # Running parameter form
│       │   ├── track-parameter/           # Track parameter form
│       │   ├── electrical-parameter/      # Electrical parameter form
│       │   ├── output/                    # Simulation results & charts
│       │   │   ├── page.tsx               # Tab container + export logic
│       │   │   ├── speed-tab.tsx
│       │   │   ├── power-tab.tsx
│       │   │   ├── current-tab.tsx
│       │   │   ├── force-tab.tsx
│       │   │   ├── distance-tab.tsx
│       │   │   └── power-per-motor-tab.tsx
│       │   └── optimization/              # Fuzzy sweep UI
│       ├── components/
│       │   ├── sidebar/                   # Navigation sidebar
│       │   ├── charts/                    # Recharts wrappers
│       │   ├── inputs/                    # Reusable form input widget
│       │   └── ui/                        # shadcn/ui components
│       ├── services/
│       │   └── api.ts                     # All HTTP calls to backend
│       ├── types/
│       │   ├── input-params.ts            # TypeScript types for form data
│       │   └── simulation-params.ts       # Types for simulation results
│       ├── contexts/
│       │   └── FormPersistenceContext.tsx # localStorage form persistence
│       └── lib/
│           ├── qt-webchannel.ts           # Qt WebChannel bridge (file dialogs)
│           └── backendInit.ts             # Boot sequence: push defaults to backend
│
├── backend/                     # Qt C++ application
│   ├── main.cpp                           # Entry point (--headless / --dev / --port flags)
│   ├── core/
│   │   └── appcontext.{h,cpp}             # Singleton: owns all data models
│   ├── models/                            # Plain structs (no Qt dependency)
│   │   ├── train_data.h
│   │   ├── mass_data.h
│   │   ├── power_data.h
│   │   ├── energy_data.h
│   │   ├── resistance_data.h
│   │   ├── simulation_data.h              # Time-series result arrays
│   │   └── ...
│   ├── http_server/
│   │   ├── http_server.{h,cpp}            # Qt HttpServer, port 8080
│   │   ├── api_handler.{h,cpp}            # Route dispatcher
│   │   ├── inputs/                        # Per-parameter HTTP handlers
│   │   └── simulations/
│   │       └── simulation_handler.*       # /api/simulation/* routes
│   ├── controllers/
│   │   ├── simulation/                    # Physics engine
│   │   │   ├── train_simulation_handler.* # Main simulation loop
│   │   │   ├── mass_handler.*
│   │   │   ├── power_handler.*
│   │   │   ├── current_handler.*
│   │   │   ├── energy_handler.*
│   │   │   ├── resistance_handler.*
│   │   │   ├── tractive_effort_handler.*
│   │   │   └── ...
│   │   ├── optimization/
│   │   │   ├── optimization_handler.*     # Parameter sweep + scoring
│   │   │   └── fuzzy/                     # Mamdani fuzzy engine
│   │   │       ├── fuzzy_engine.*
│   │   │       ├── fuzzy_variable.*
│   │   │       ├── fuzzy_set.*
│   │   │       ├── triangle_set.*
│   │   │       └── trapezoid_set.*
│   │   └── output/
│   │       └── csv_output_handler.*
│   └── webengine/
│       ├── webengine_window.{h,cpp}       # QMainWindow with embedded QWebEngineView
│       └── file_bridge.{h,cpp}            # Qt WebChannel file save slots
│
├── scripts/
│   ├── dev.sh                             # Development runner
│   └── run.sh                             # Production runner
├── package.json                           # Root npm workspace
├── build.sh                               # Full build script (debug / release / prod)
├── legends.md                             # Variable naming conventions
└── documentations/
    ├── DEVELOPER_GUIDE.md                 # Architecture & API deep-dive
    ├── FUZZY_LOGIC_IMPLEMENTATION.md      # Fuzzy engine details mapped to thesis
    └── FUZZY_OPTIMIZATION_STRATEGY.md    # Optimization strategy
```

---

## Getting Started

### Prerequisites

| Tool    | Minimum version | Purpose                                  |
| ------- | --------------- | ---------------------------------------- |
| Qt      | 6.7             | Backend runtime + WebEngine + HttpServer |
| CMake   | 3.16            | Backend build                            |
| Ninja   | any             | Fast incremental builds                  |
| Node.js | 18              | Frontend dev server & build              |
| npm     | 9               | Package management                       |

**Qt modules required:** `Core`, `Network`, `HttpServer`, `Widgets`, `WebEngineWidgets`, `WebChannel`, `Concurrent`

**Fedora / RHEL:**

```bash
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel \
                 qt6-qthttpserver-devel qt6-qtwebchannel-devel \
                 cmake ninja-build nodejs npm
```

**Ubuntu / Debian:**

```bash
sudo apt install qt6-base-dev qt6-webengine-dev \
                 cmake ninja-build nodejs npm
```

**Windows:** Install Qt 6.7+ via the Qt online installer. Check:

- Qt 6.x.x with `Qt WebEngine`, `Qt WebChannel`, `Qt HttpServer`
- CMake 3.x
- Ninja
- LLVM-MinGW 64-bit or MSVC

### Clone & Install

```bash
git clone https://github.com/illufoxKusanagi/Train-Simulation-App.git
cd Train-Simulation-App

# Install frontend dependencies
cd frontend && npm install && cd ..
```

---

## Development Workflow

### Run in Development Mode

```bash
# Starts both Qt backend (port 8080) and Next.js dev server (port 3254)
npm run dev

# Or individually:
npm run dev:backend    # Qt backend only
npm run dev:frontend   # Next.js only (hot reload)
```

The Qt window loads `http://localhost:3254` in dev mode. Frontend changes hot-reload inside the window instantly.

### Build for Production

```bash
# Full production build: backend Release binary + frontend static export
./build.sh prod

# Or step by step:
npm run build:backend    # CMake Release build → backend/build/bin/
npm run build:frontend   # next build → frontend/out/
```

The production binary serves the static Next.js export directly — no separate Node process needed.

### VS Code Tasks

Pre-configured tasks in `.vscode/tasks.json`:

| Task                               | Action                          |
| ---------------------------------- | ------------------------------- |
| `CMake: Configure (Debug/Release)` | Configure backend CMake         |
| `CMake: Build`                     | Compile the backend             |
| `CMake: Clean`                     | Clean backend build artifacts   |
| `Build: Frontend`                  | `npm run build` in `frontend/`  |
| `Build: All (Debug/Release)`       | `./build.sh debug` or `release` |
| `Run: Application`                 | Launch compiled app             |
| `Run: Development Mode`            | `./dev.sh`                      |

### Add a New Frontend Page

1. Create `frontend/src/app/your-page/page.tsx`.
2. Wrap content in `<PageLayout>` for consistent sidebar + header.
3. Add a navigation entry in `frontend/src/components/sidebar/app-sidebar.tsx`.
4. Add any new API calls to `frontend/src/services/api.ts` and matching types in `frontend/src/types/`.

### Add a New API Endpoint

1. Add the route handler method to the appropriate class in `backend/http_server/inputs/` or create a new handler file.
2. Register the route in `backend/http_server/http_server.cpp` inside `setupRoutes()`.
3. Wire the handler in `backend/http_server/api_handler.{h,cpp}`.
4. Add the corresponding `fetch` call in `frontend/src/services/api.ts`.

### Variable Naming Conventions

Follow the prefix table in [`legends.md`](legends.md). Both backend C++ structs and frontend TypeScript types use the same names so the JSON round-trip is direct.

| Prefix  | Meaning                    | Unit         |
| ------- | -------------------------- | ------------ |
| `n_`    | Count / quantity           | —            |
| `mass_` | Mass                       | kg           |
| `f_`    | Force                      | kN           |
| `v_`    | Speed                      | km/h         |
| `p_`    | Power                      | kW           |
| `e_`    | Energy                     | kWh          |
| `eff_`  | Efficiency                 | 0–1 fraction |
| `vol_`  | Voltage                    | V            |
| `curr_` | RMS current                | A            |
| `x_`    | Distance                   | m            |
| `r_`    | Resistance                 | kN           |
| `stat_` | Static-simulation variable | varies       |
| `tm_`   | Per traction motor         | varies       |

---

## API Reference

All endpoints: `http://localhost:8080`. CORS is open for the frontend at port 3254.

### System

| Method | Path                 | Description                           |
| ------ | -------------------- | ------------------------------------- |
| GET    | `/api/health`        | Backend liveness check                |
| POST   | `/api/init/quick`    | Push default values into `AppContext` |
| GET    | `/api/debug/context` | Dump full `AppContext` state as JSON  |

### Parameters

| Method   | Path                         | Payload key            |
| -------- | ---------------------------- | ---------------------- |
| GET/POST | `/api/parameters/train`      | `trainParameters`      |
| GET/POST | `/api/parameters/electrical` | `electricalParameters` |
| GET/POST | `/api/parameters/running`    | `runningParameters`    |
| GET/POST | `/api/parameters/track`      | `trackParameters`      |
| GET/POST | `/api/parameters/mass`       | `massParameters`       |

All POSTs accept `Content-Type: application/json` and return `{ status, message }`.

### Simulation

| Method | Path                      | Description                                                 |
| ------ | ------------------------- | ----------------------------------------------------------- |
| POST   | `/api/simulation/start`   | Start simulation. Body: `{ "type": "static" \| "dynamic" }` |
| GET    | `/api/simulation/status`  | `{ isRunning, summary }`                                    |
| GET    | `/api/simulation/results` | Full time-series `SimulationDataPoint[]`                    |

### Optimization

| Method | Path                       | Description                                                                |
| ------ | -------------------------- | -------------------------------------------------------------------------- |
| POST   | `/api/optimization/start`  | Start fuzzy sweep with parameter ranges                                    |
| GET    | `/api/optimization/status` | `{ isRunning, completedCombinations, totalCombinations, results[], best }` |

---

## Simulation Engine

The physics engine in `backend/controllers/simulation/train_simulation_handler.cpp` uses a fixed time-step integration loop.

### Phases

Each inter-station trip is divided into phases managed by `MovingHandler`:

1. **Powering** — Motor force applied, train accelerates through notch speed points.
2. **Coasting** — No motor force; speed decays naturally.
3. **Braking** — Regenerative/mechanical braking to stop at the next station.

### Per-step Calculation

$$R = R_{\text{Davis}} + R_{\text{grade}} + R_{\text{curve}}$$

$$a = \frac{F_{\text{motor}} - R}{M_{\text{eff}}}$$

$$v_{t+1} = v_t + a \cdot \Delta t, \quad x_{t+1} = x_t + v_t \cdot \Delta t$$

$$E \mathrel{+}= P_{\text{catenary}} \cdot \Delta t$$

$M_{\text{eff}}$ includes rotational inertia of motors and wheels.

### Output Variables

Every step appends to `SimulationDatas` arrays. The full set is defined in [`frontend/src/types/simulation-params.ts`](frontend/src/types/simulation-params.ts): speeds, forces, powers, currents, energies, distances, and per-motor values.

---

## Fuzzy Optimization

The optimization page sweeps a user-defined grid of `(acc_start_si, v_p1)` combinations. Each point runs a full simulation and is scored by the Mamdani fuzzy engine.

### Fuzzy Variables

| Variable        | Role   | Sets                                                |
| --------------- | ------ | --------------------------------------------------- |
| Acceleration    | Input  | Low / Medium / High (Triangle)                      |
| Weakening Point | Input  | Low / Medium / High (Triangle)                      |
| Gradient        | Input  | Low / Medium / High (Trapezoid)                     |
| Speed Limit     | Input  | Low / Medium / High (Trapezoid)                     |
| Suitability     | Output | Poor / Fair / Suitable / Quite Suitable / Excellent |

**AND:** Minimum | **Aggregation:** Maximum | **Defuzzification:** Centroid

### Score Labels

| Range | Label     |
| ----- | --------- |
| ≥ 75  | Excellent |
| ≥ 50  | Good      |
| ≥ 25  | Fair      |
| < 25  | Poor      |

For implementation details mapped to the thesis equations, see [`documentations/FUZZY_LOGIC_IMPLEMENTATION.md`](documentations/FUZZY_LOGIC_IMPLEMENTATION.md).

---

## Contributing

1. Fork the repository and create a feature branch from `main`.
2. Read [`documentations/DEVELOPER_GUIDE.md`](documentations/DEVELOPER_GUIDE.md) before touching backend code.
3. Keep TypeScript types in `frontend/src/types/` in sync with any C++ model changes — the JSON field names must match exactly.
4. Follow the naming conventions in [`legends.md`](legends.md).
5. Open a pull request against `main`.

---

_Licensed under the [MIT License](LICENSE)._
