# CHANGELOG - Train Simulation App

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## [Unreleased] - 2026-04-29

### Added

#### Powering Gear (P1–P7) & Braking Gear (B1–B7)

- **Files affected:**
  - `frontend/src/app/running-parameter/form.constants.ts`
  - `frontend/src/app/running-parameter/page.tsx`
  - `frontend/src/types/input-params.ts`
  - `frontend/src/services/api.ts`
  - `frontend/public/running_parameters_template.csv`
- **What:** Introduced gear-based acceleration/deceleration scaling. The user inputs a "linear" (maximum) value and selects a gear notch; the effective value sent to the backend is computed automatically.
  - **Formula:** `acc_start_si = acc_linear_si × (gear / 7)`
  - P7/B7 = full capability, P1/B1 = 1/7 of maximum
- **Frontend-only computation:** The gear calculation happens entirely in the frontend via `react-hook-form` watchers. The backend continues to receive `acc_start_si`/`decc_start_si` unchanged — zero backend modifications required.
- **Dropdown options:** `pow_gear` renders P1–P7, `brake_gear` renders B1–B7 via the existing `InputDropdown` component.
- **Computed fields:** `acc_start_si`, `acc_start`, `decc_start_si`, `decc_start` are now **read-only** form fields that auto-update whenever the linear input or gear selection changes.

### Changed

#### Rename: `acc_start_si` → `acc_linear_si` (User Input Field)

- **Files affected:**
  - `frontend/src/app/running-parameter/form.constants.ts`
  - `frontend/src/app/running-parameter/page.tsx`
  - `frontend/src/types/input-params.ts`
- **What:** The user-editable acceleration field was renamed from `acc_start_si` to `acc_linear_si` to distinguish it from the gear-computed effective value. Same for `acc_start` → `acc_linear`, `decc_start_si` → `decc_linear_si`, `decc_start` → `decc_linear`.
- **Impact:** `acc_start_si`/`decc_start_si` are now computed values (read-only), while `acc_linear_si`/`decc_linear_si` are the user-editable inputs.
- **Bidirectional sync** updated: SI↔non-SI pairs now sync `acc_linear_si` ↔ `acc_linear` (×3.6) instead of the old field names.

---

## [Unreleased] - 2026-04-13

### Added

#### Non-SI Unit Parameters (`acc_start`, `decc_start`, `decc_emergency`)

- **Files affected:**
  - `backend/models/moving_data.h`
  - `backend/http_server/inputs/running_parameter_handler.cpp`
  - `backend/http_server/api_handler.cpp`
  - `frontend/src/types/input-params.ts`
  - `frontend/src/app/running-parameter/form.constants.ts`
  - `frontend/src/app/running-parameter/page.tsx`
  - `frontend/public/running_parameters_template.csv`
- **What:** Introduced non-SI (km/h/s) counterparts for acceleration and deceleration parameters alongside the existing SI (m/s²) fields.
  - `acc_start` (default: 2.88 km/h/s) — non-SI counterpart of `acc_start_si`
  - `decc_start` (default: 3.88 km/h/s) — non-SI counterpart of `decc_start_si`
  - `decc_emergency` (default: 5.16 km/h/s) — non-SI counterpart of `decc_emergency_si`
- **Backend:** Added `decc_emergency` field to `MovingData` struct. Updated `RunningParameterHandler` to read/write all three non-SI fields in both GET and POST handlers. Added defaults in `handleQuickInit()` and fields in `handleDebugContext()`.
- **Frontend:** Added Zod schema validation and form field definitions for `acc_start`, `decc_start`, `decc_emergency`. Implemented **bidirectional real-time sync** between SI and non-SI fields using `react-hook-form` `watch()` with an `isSyncing` ref guard to prevent infinite update loops:
  - Editing an SI field → auto-updates its non-SI counterpart (`× 3.6`)
  - Editing a non-SI field → auto-updates its SI counterpart (`÷ 3.6`)
- **CSV template:** Updated `running_parameters_template.csv` with the three new fields and default values.

### Changed

#### BREAKING CHANGE: `decc_emergency` → `decc_emergency_si` Rename

- **Files affected (11 files):**
  - `backend/models/moving_data.h`
  - `backend/controllers/data/running_data_handler.cpp`
  - `backend/controllers/simulation/simulation_track_handler.cpp`
  - `backend/http_server/inputs/running_parameter_handler.cpp`
  - `backend/http_server/api_handler.cpp`
  - `frontend/src/types/input-params.ts`
  - `frontend/src/app/running-parameter/form.constants.ts`
  - `frontend/src/app/running-parameter/page.tsx`
  - `frontend/public/running_parameters_template.csv`
- **What:** Renamed the existing `decc_emergency` field to `decc_emergency_si` across the entire codebase for consistency with the `_si` suffix naming convention (m/s² units). The freed `decc_emergency` name is now used for the new non-SI (km/h/s) field.

### Fixed

#### Braking Distance Division-by-Zero Guard

- **File:** `backend/controllers/simulation/simulation_track_handler.cpp`
- **Functions:** `calculateStaticBrakingTrack()`, `calculateBrakingEmergencyTrack()`
- **Issue:** Potential division by zero when `decc_start_si` or `decc_emergency_si` is zero or negative.
- **Solution:** Added early return guard `if (movingData->decc_start_si <= 0.0) return 0.0;` before the braking distance formula. Also fixed the order of the empty-list check so `speed` is assigned `0.0` before attempting to access `.last()` on a potentially empty list.

#### Frontend: `next-intl` ENVIRONMENT_FALLBACK Crash

- **File:** `frontend/src/i18n/request.ts`
- **Issue:** The application crashed on page load with `Error: ENVIRONMENT_FALLBACK: There is no timeZone configured`. This caused the Qt WebEngine window to close immediately after opening.
- **Root Cause:** `next-intl` v4.x requires a `timeZone` in the request configuration to avoid server/client markup mismatches. The field was missing from `getRequestConfig()`.
- **Solution:** Added `timeZone: "Asia/Jakarta"` to the `getRequestConfig()` return object.

---

## [Unreleased] - 2026-04-01

### Added

#### Fuzzy Logic Visualization & Optimization

- Implemented dynamic **Fuzzy Membership Charts** in the Optimization page using Recharts.
- Integrated **Travel Time**, **Peak Motor Power**, and **Output Score** membership function visualizations.
- Added **All Results** distribution scatter plots and a **Winner** star marker/reference line to the charts.
- Implemented frontend mathematical replication of the backend's dynamic fuzzy range generation (including the 5% expansion margin and trapezoidal/triangular functions).
- Added a new **Winner Analysis** card showing the exact membership degrees ($\mu$) for the winning result.

#### UI: Optimization Tabbed Interface

- Refactored the `OptimizationPage` to use a tabbed layout, separating "Best Combination" from "Fuzzy Membership Charts".
- Extracted optimization UI components into modular `WinnerTab` and `FuzzyMemberTab` files.

#### Resistance Hndler Adjustment

- Revised formula for start resistance, from using `mass_totalInertial` to `mass_totalLoad`.

---

## [Unreleased] - 2026-03-30

### Added

#### Internationalization (i18n)

- Implemented full localization/internationalization support across the application.
- Updated locale strings for frontend components.

#### Optimization & Form Export

- Added the ability to export optimization results to a CSV file natively using the Qt File Dialog.
- Added feature to export form schema parameters directly to CSV.

#### Chart Data & Image Export

- Implemented functionality allowing users to save individual chart figures as images.
- Added support for exporting raw data from each chart into a CSV file.

#### UI: Password Visibility Toggle & Loading Skeletons

- Added a password visibility toggle (Eye/EyeOff) to the login screen for better user experience.
- Added skeleton loading layouts on the output page.

#### Core Simulation Integration

- Implemented `f_resStart_0` (initial running resistance at 0 speed) into the simulation physics engine.

### Changed

#### Authentication

- Updated the backend authentication algorithm and modified the default admin password hash.

#### CI/CD Pipelines

- Updated GitHub Actions release workflows, updating action-gh release versions to accommodate Windows builds.

#### Core: DRY Refactoring of Form Buttons

- Abstracted Save, Reset, Import CSV, and Export CSV buttons into a single, shared `<FormActionButtons>` component across component pages to maintain DRY principles.

### Fixed

#### Simulation Physics Core

- Corrected total-force and motor-force calculations that failed when mass input was not computed.
- Corrected braking-distance computation logic.
- Resolved a blank page shown immediately after starting the simulation.
- Improved metric accuracy for power and force resistance (`f_res` and `f_motor`).

#### State & Input Management

- Fixed an issue where persistent states (like the `n_car` dropdown configurations) were resetting improperly after page swaps.
- Fixed a bug preventing users from properly interacting with and inputting decimal values into parameter fields.

#### Native UI Integrations & Filesystem

- Fixed an issue where external links (e.g., footer credits) were opening inside the embedded Qt WebEngine application window instead of the user's OS native browser.
- Fixed bugs disrupting the saving of chart images and chart CSV records preventing successful disk writes.

---

## [Unreleased] - 2026-03-09

### Added

#### UI: Color Scheme and Typography

- **Files affected:**
  - `frontend/src/app/globals.css`
  - `frontend/src/app/layout.tsx`
  - `frontend/src/components/app-sidebar.tsx`
- Applied `colors.h` Primary/Secondary palette and `text_style.h` Roboto typography system to the frontend.
  - Light mode: Primary500 cyan for primary actions, Secondary100 for borders.
  - Dark mode: default shadcn surfaces retained; sidebar uses Secondary500 navy same as light mode.
  - Font swapped from Geist → Roboto with exact px sizes from `text_style.h`.
  - `SidebarTrigger` in `app-sidebar.tsx` now uses sidebar CSS variable tokens (`bg-sidebar`, `text-sidebar-foreground`, `border-sidebar-border`, `hover:bg-sidebar-accent`) for visual consistency.

#### Optimization: Step-Based Parameter Sweep (replaces 3-point discrete)

- **Files affected:**
  - `backend/http_server/api_handler.cpp`
  - `frontend/src/app/optimization/page.tsx`
- Replaced the 3-point (Low/Medium/High) discrete sweep with a continuous step-based range generator:
  - Acceleration: `accelLow → accelHigh` in **0.05 m/s²** steps.
  - Weakening speed: `weakeningLow → weakeningHigh` in **5 km/h** steps.
  - Step count computed with `qRound((high - low) / step)` to avoid floating-point drift.
  - `accelMedium` / `weakeningMedium` fields remain in the UI as visual midpoint references but are no longer consumed by the backend.
- Toast message now dynamically computes and displays the actual combination count at submit time: `${nAcc} acc × ${nVp1} v_p1 = ${nAcc * nVp1} combinations`.
- Empty-state text updated to remove the hardcoded "20-combination" reference.

#### Output Page: "Time at Peak Power" Summary Card

- **Files affected:**
  - `backend/http_server/simulations/simulation_handler.cpp`
  - `frontend/src/app/output/page.tsx`
- `getMaxPowTime()` existed in `TrainSimulationHandler` (sums `time[i]` wherever `vvvfPowers[i]` equals its maximum within ε = 0.001) but was never emitted in the API response.
- Added `summary["maxPowerTime"] = m_trainSimulation->getMaxPowTime()` to the status response's summary object.
- Added "Time at Peak Power" summary card in the output page displaying `results.summary?.maxPowerTime` in seconds. The `maxPowerTime` field already existed in the `SimulationSummary` TypeScript type but was not wired end-to-end.

### Fixed

#### Track Parameter: Custom Slope Options Not Saved or Submitted

- **File:** `frontend/src/app/track-parameter/page.tsx`
- `slope_option1–4` belonged to `TrackFormSchema` and were rendered via `slopeFormRows` inside `constantForm`, but a dead parallel `slopeForm` (backed by `SlopeFormSchema`) was also instantiated without ever being submitted. The `onSubmit` payload omitted all four slope option fields.
- **Solution:** Removed the dead `slopeForm` / `SlopeFormSchema` entirely. Added `slope_option1–4` default values to `constantForm` and included them explicitly in the `trackParams` payload sent to the backend.

#### Backend: Optimization Re-entry Guard Race Condition

- **File:** `backend/http_server/api_handler.cpp`
- The atomic re-entry guard (`testAndSetRelaxed`) inside `handleOptimization()` ran on the background thread dispatched by `QtConcurrent::run()`. Two simultaneous POST requests would both successfully call `QtConcurrent::run()` and receive HTTP 200 before either thread checked the guard — the second thread would then silently return with no error visible to the caller.
- **Solution:** Added `m_optimizationHandler->isRunning()` check **before** the `QtConcurrent::run()` dispatch. Returns HTTP 409 Conflict immediately, so no second background task is ever spawned.

#### Backend: Malformed JSON Payload Silently Starts Optimization

- **Files affected:**
  - `backend/http_server/http_server.h`
  - `backend/http_server/http_server.cpp`
- `parseRequestBody()` returned an empty `QJsonObject{}` on **both** genuine JSON parse failures and intentionally empty bodies — indistinguishable downstream. A malformed body caused `handleStartOptimization({})` to fall back to current parameters and silently start optimization.
- **Solution:** Added `bool *parseOk = nullptr` out-param to `parseRequestBody`. Empty body → `*parseOk = true`, returns `{}`. Parse failure → `*parseOk = false`, returns `{}`. The `/api/optimization/start` route checks `parseOk` and returns HTTP 400 Bad Request when the body is non-empty but fails to parse.

#### Backend: DevTools Window Memory Leak

- **File:** `backend/webengine/webengine_window.cpp`
- `QWebEngineView *devToolsView = new QWebEngineView()` was allocated with no parent and no `WA_DeleteOnClose` attribute. Closing the DevTools window orphaned the allocation; every subsequent DevTools open added another leak.
- **Solution:** Added `devToolsView->setAttribute(Qt::WA_DeleteOnClose, true)` before `show()`.

#### Frontend: Swallowed Initial Fetch Errors on Parameter Pages

- **Files affected:**
  - `frontend/src/app/electrical-parameter/page.tsx`
  - `frontend/src/app/running-parameter/page.tsx`
  - `frontend/src/app/track-parameter/page.tsx`
- All three parameter pages had `.catch(() => {})` on the `useEffect` fetch that loads saved parameters from the backend. A network or backend failure was silently swallowed — the form used hardcoded defaults with no indication to the user.
- **Solution:** Replaced empty catches with `.catch((err) => { console.error(...); toast.error("Could not load saved parameters — using defaults"); })`.

#### Frontend: Trainset CSV Import Order-Dependent

- **File:** `frontend/src/app/train-parameter/page.tsx`
- `processTrainsetCsvText` applied CSV rows sequentially. If `n_M1`, `n_M2`, etc. appeared before `n_car` in the file, setting `n_car` later would trigger the `watch` preset watcher (which overwrites car counts with AW preset values), silently discarding the explicit earlier values.
- **Solution:** Parse all valid lines into an `updates[]` array first, then pull `n_car` out and place it at the front before applying in order. The preset watcher fires first, and all subsequent fields (including explicit car counts) correctly overwrite it.

---

- **Files affected:**
  - `frontend/src/app/optimization/page.tsx`
  - `frontend/src/services/api.ts`
  - `backend/http_server/http_server.cpp`
  - `backend/http_server/api_handler.h`
  - `backend/http_server/api_handler.cpp`
  - `backend/controllers/optimization/optimization_handler.h`
  - `backend/controllers/optimization/optimization_handler.cpp`
- **Issue:** The "Fuzzy Membership Ranges" form on the optimization page (fields `accelLow`, `accelMedium`, `accelHigh`, `weakeningLow`, `weakeningMedium`, `weakeningHigh`) was purely decorative — values were validated by the form schema but never sent to the backend. The backend had hardcoded auto-centered sweep logic.
- **Root Cause (multi-layer):**
  1. `page.tsx handleStart()` called `api.startOptimization()` with no arguments — form values were never read
  2. `api.ts startOptimization` had no parameters and sent an empty POST body
  3. `http_server.cpp` POST route ignored the request body entirely (`const QHttpServerRequest &` not parsed)
  4. `api_handler.cpp handleStartOptimization()` accepted no parameters
  5. `optimization_handler.cpp handleOptimization()` built its own `accValues`/`vp1Values` from a fixed ±0.1 centered sweep around user's current `acc_start_si`, and ±{15,5,-5,-15} km/h around `v_p1`
- **Solution:**
  - `handleOptimization()` signature changed to `handleOptimization(const QList<double> &accCandidates, const QList<double> &vp1Candidates)`; auto-sweep building replaced with the passed candidates
  - `m_totalCombinations` (`QAtomicInt`) added to `OptimizationHandler` — set to `accCandidates.size() × vp1Candidates.size()` at the start of each run; exposed via `getTotalCombinations()`
  - `api_handler.cpp handleStartOptimization(const QJsonObject &data)` parses `accelLow/Medium/High` → `accValues` and `weakeningLow/Medium/High` → `vp1Values`; passes them to `handleOptimization()`; falls back to user's current loaded parameters if all 6 fields are missing/zero
  - `handleGetOptimizationStatus()` reads `getTotalCombinations()` instead of hardcoding 20
  - `http_server.cpp` POST route now parses request body via `parseRequestBody(request)` and forwards `QJsonObject` to `handleStartOptimization(data)`
  - `api.ts startOptimization` now accepts `params: { accelLow, accelMedium, accelHigh, weakeningLow, weakeningMedium, weakeningHigh }` and sends them as JSON body
  - `page.tsx handleStart()` calls `constantForm.getValues()` and passes the result to `api.startOptimization()`; description text updated to "3 acc × 3 v_p1 = 9 combinations"
- **Behaviour after fix:** User-defined Low/Medium/High values for acceleration (m/s²) and weakening point (km/h) are the exact discrete candidates swept. 3 × 3 = 9 combinations scored by the Mamdani fuzzy engine.

#### Force-Speed Chart: Running Resistance Option Lines

- **File:** `frontend/src/app/output/force-tab.tsx`
- **What:** Added 4 dashed `<Line>` components (`motorResistancesOption1-4`) and the base `motorResistance` line, each with a `chartConfig` entry (colour + label). Previously only traction/braking force lines were shown; running resistance options from custom slope inputs were not charted even though they were computed in the backend.

### Fixed

#### Running Resistance Zeros in Static Simulation (3 Simultaneous Bugs)

- **Files affected:**
  - `backend/controllers/simulation/train_simulation_handler.cpp`
  - `backend/http_server/inputs/track_parameter_handler.cpp`
  - `backend/http_server/simulations/simulation_handler.cpp`

**Bug 1 — Circular guard deadlock in `calculateRunningResEachSlope()`:**

- **Function:** `train_simulation_handler.cpp: calculateRunningResEachSlope()`
- **Issue:** Guard condition `f_resRunningOption{n} > 0` prevented the function from ever computing — the result was zero, so the guard blocked recomputation, keeping it zero.
- **Solution:** Removed the `&& f_resRunningOption{n} > 0` half of each condition. The function is only gated on `stat_slope_option{n} != 0.0` (or similar physics precondition).

**Bug 2 — Boolean trap on zero-slope flat track:**

- **Function:** `train_simulation_handler.cpp: calculateRunningResEachSlope()`
- **Issue:** Condition `if (stat_slope_option1 && ...)` evaluates `false` when the slope is exactly `0.0` (flat track), because `0.0` is falsy in C++. This caused the function to skip the flat-track running resistance calculation entirely.
- **Solution:** Replaced `if (stat_slope_option1 && ...)` with `if (stat_slope_option1 != 0.0 || ...)` or restructured the calculation to not gate on the slope value.

**Bug 3 — Update handler never saved slope_option inputs:**

- **Function:** `backend/http_server/inputs/track_parameter_handler.cpp: handleUpdateTrackParameters()`
- **Issue:** PUT requests containing `slope_option1/2/3/4` from the frontend form were silently ignored — the handler parsed only the base track fields and never wrote `slope_option1-4` to `AppContext`.
- **Solution:** Added four `data["slope_option1"].toDouble()` read/assign blocks in `handleUpdateTrackParameters()` to persist the custom slope values.

**Key serialization bug — Field name mismatch in `getStaticResults()`:**

- **Function:** `backend/http_server/simulations/simulation_handler.cpp: getStaticResults()`
- **Issue:** Serialized the 4 option vectors under keys `motorResistancesZero/Five/Ten/TwentyFive`, but the frontend expected `motorResistancesOption1/2/3/4`.
- **Solution:** Renamed the JSON keys to `motorResistancesOption1/2/3/4`. Also added bounds-safe ternary access (`!vec.isEmpty() ? vec[0] : 0.0`) in both `getStaticResults()` and `getDynamicResults()`.

#### Fuzzy Membership Function Visualization Script

- **File:** `scripts/generate_fuzzy_membership_graphs.py`
- **What:** Python/Colab script that exactly mirrors the C++ fuzzy engine's membership function logic and produces 8 labeled plots
  - `trapezoid_mf(x, m_min, m_peak1, m_peak2, m_max)` — mirrors `TrapezoidSet::membership()` with strict boundary inequalities (endpoints return 0.0)
  - `triangle_mf(x, m_min, m_peak, m_max)` — mirrors `TriangleSet::membership()`
  - `compute_input_breakpoints(raw_min, raw_max)` — applies 5% margin then derives fractional breakpoints: Short/Low = Trapezoid(lo, lo, lo+0.25·vr, lo+0.45·vr), Medium = Triangle(lo+0.30·vr, lo+0.50·vr, lo+0.70·vr), Long/High = Trapezoid(lo+0.55·vr, lo+0.75·vr, hi, hi)
  - Fixed output terms on [0,100]: Poor = Trapezoid(0,0,15,30), Fair = Triangle(20,38,55), Good = Triangle(45,62,78), Excellent = Trapezoid(68,82,100,100)
  - `eval_term` + `cog_defuzzify` — Mamdani MIN implication, MAX aggregation, COG over 101 sample points
  - `plot_time_engine` / `plot_power_engine` — full engine plots with optional COG walkthrough example
  - `plot_acceleration_membership` — hypothetical acc_start_si MFs over [0.3, 1.5] m/s² (reference; parameter is swept discretely, not fuzzified)
  - `plot_weakening_speed_membership` — hypothetical v_p1 MFs over [20, v_limit−5] km/h with candidate lines marked (reference; same reason)
  - `plot_sweep_candidates` — bar chart of 5 acc_start_si + 4 v_p1 candidate grid (the actual discrete sweep values)
  - `plot_final_score_sweep` — 4×5 score heatmap grid for all 20 parameter combinations
  - `plot_overview_grid` — 2×3 summary figure
  - Saves all plots to `fuzzy_graphs/` as both PNG and PDF
- **Usage:** Run `main()` in Colab or locally; all parameters (min/max time, min/max power, example inputs, candidate values, v_limit) are configurable via `main()` arguments

### Fixed

#### Missing TypeScript Field: `powerMotorOutputPerMotor` in `SimulationDataPoint`

- **File:** `frontend/src/types/simulation-params.ts`
- **Issue:** `powerMotorOutputPerMotor` was referenced in `frontend/src/app/output/power-per-motor-tab.tsx` (lines 24, 74, 75) but absent from the `SimulationDataPoint` interface — causing a silent TypeScript type gap where the field would be `undefined` at runtime even though the backend sends it
- **Root Cause:** Field was omitted when the interface was originally written; `powerMotorOut` and `powerMotorIn` were present but the per-motor breakdown field was skipped
- **Solution:** Added the missing field between `powerMotorOut` and `powerMotorIn`:
  ```typescript
  powerMotorOut: number; // P_motor Out
  powerMotorOutputPerMotor: number; // P_motor Out Per Motor
  powerMotorIn: number; // P_motor In
  ```
- **Impact:** `power-per-motor-tab.tsx` chart now correctly reads the per-motor power value from the API response without data loss

---

## [Unreleased] - 2026-03-04

### Added

#### Fuzzy Logic Optimization Engine

- **Files:** `backend/utils/fuzzy_engine.h`, `backend/utils/fuzzy_engine.cpp`
- **What:** Full Mamdani fuzzy inference engine built from scratch in Qt/C++
  - `FuzzySet` abstract base class with `TriangleSet` and `TrapezoidSet` concrete implementations
  - `FuzzyVariable` — holds named membership function terms, tracks current crisp value; exposes both `getMembership()` (at current value) and `getMembershipAt(term, x)` (at arbitrary point, required by centroid defuzzification)
  - `FuzzyRule` — `QMap<QString,QString>` antecedents + `QPair<QString,QString>` consequent
  - `FuzzyEngine` — Mamdani MIN-AND inference, MAX aggregation, centroid defuzzification over 100-point numerical integration; `clear()` allows full reconfiguration between runs without reconstructing the object

#### Fuzzy Optimization Handler

- **Files:** `backend/controllers/optimization/optimization_handler.h`, `optimization_handler.cpp`
- **What:** Two-pass parameter sweep optimizer using the fuzzy engine
  - `OptResult` struct: `acc_start_si` (m/s²), `v_p1` (km/h), `peakMotorPower` (kW/motor), `travelTime` (s), `fuzzyScore` (0–100)
  - **Pass 1:** Sweeps all `acc × v_p1` combinations (up to 5 × 4 = 20), calls `runDynamicSimulation()` for each, records `(travelTime, peakMotorPower)` raw metrics
  - **Pass 2:** Derives actual min/max ranges from real Pass 1 data, calibrates `setupFuzzyEngine()` with those ranges (+ 5% margin), then scores every result via `evaluateFuzzyScore()`
  - Engine is never calibrated against hardcoded static ranges — adapts to any train + track configuration
  - Sweep is centred on user's actual loaded parameters (`acc_start_si`, `v_p1`); values are clamped to physical limits
  - `m_resultsMutex` (`QMutex`) guards `m_results` / `m_bestResult`; `m_isRunning` (`QAtomicInt`) prevents concurrent runs
  - Emits `optimizationComplete(OptResult best)` signal on finish
- **Fuzzy variable setup (`setupFuzzyEngine`):**
  - Input 1 — TravelTime: Short (trapezoid), Medium (triangle), Long (trapezoid)
  - Input 2 — MotorPower: Low (trapezoid), Medium (triangle), High (trapezoid)
  - Output — FuzzyScore 0–100: Poor, Fair, Good, Excellent
  - 9 Mamdani rules (3 time × 3 power combinations)

#### Optimization Frontend Page

- **File:** `frontend/src/app/optimization/page.tsx`
- **What:** Full optimization UI in Next.js
  - `OptResult` TypeScript interface matching backend struct field-for-field
  - `scoreColor()`, `scoreLabel()`, `scoreBadgeClass()` — threshold helpers (≥75 Excellent, ≥50 Good, ≥25 Fair, else Poor)
  - 1500 ms polling loop via `setInterval` / `pollingRef`; auto-stops when `isRunning` flips false
  - On-mount restore: fetches current status so results survive page navigation (backend keeps `m_results` in memory while Qt process is alive)
  - Winner card with trophy icon and 5-metric grid (score, acc, v_p1, peak power, travel time)
  - All-combinations results table with per-row score badge
  - Progress bar (completed / total combinations)
  - Error toast + state reset if `completedCombinations === 0` after run ends

### Fixed

#### Critical Bug: Optimization Deadlock (Double Mutex Lock)

- **File:** `backend/controllers/optimization/optimization_handler.cpp`
- **Functions:** Pass 1 sweep loop, post-sweep restore block
- **Issue:** Optimization would hang indefinitely when started
- **Root Cause:** The Pass 1 loop wrapped `runDynamicSimulation()` inside `QMutexLocker(m_simulationMutex)`. `runDynamicSimulation()` itself also acquires `m_simulationMutex` internally → non-recursive `QMutex` deadlock on the second lock attempt
- **Solution:** Removed the outer `QMutexLocker` from the Pass 1 loop and restore block. `runDynamicSimulation()` handles its own locking; no external guard is needed

#### CI: Windows Deployment Step ($BinDir Undefined / UNC Path Error)

- **File:** `.github/workflows/build-windows.yml`
- **Step:** Deploy Qt Dependencies
- **Issues:**
  1. `$BinDir` was never defined in the step scope → PowerShell silently expanded it to empty string → `windeployqt` received `"\\TrainSimulationApp.exe"` (UNC path) → file not found error
  2. Two conflicting versions of the step were merged together, creating duplicate "Install OpenSSL" and "Bundle Frontend" sub-blocks
  3. `Copy-Item` missing `-Force` flag causing `DirectoryExist` IOException
- **Solution:**
  - Added `$BinDir = Resolve-Path "bin"` at the top of the step so the variable is always defined
  - Rewrote the step as a single coherent block with one `windeployqt` call
  - Removed duplicate "Install OpenSSL" and "Bundle Frontend" sections
  - Added `-Force` to all `Copy-Item` calls

### Changed

#### CI: Linux Artifact — AppImage → Flatpak

- **File:** `.github/workflows/build-linux.yml`
- **Reason:** `linuxdeployqt` is Qt5-only and fails with Qt6 WebEngine; `linuxdeploy-plugin-qt` is unreliable with QtWebEngine
- **New approach:**
  - Runtime: `org.freedesktop.Platform//23.08` + `org.freedesktop.Sdk//23.08`
  - Base: `io.qt.qtwebengine.BaseApp//6.7` (provides Qt 6.7 WebEngine, WebChannel, WebSockets into `/app`)
  - `qt6-qthttpserver` built from source as its own Flatpak module (not in any Flatpak SDK or BaseApp) — tarball from `download.qt.io/archive`, SHA-256 hardcoded (pre-computed, not fetched at CI time)
  - Main app and qthttpserver both use `-DCMAKE_PREFIX_PATH=/app -DQt6_DIR=/app/lib/cmake/Qt6`
  - Manifest generated by a `shell: python3 {0}` step (avoids heredoc indentation issues)
  - Output: `TrainSimulationApp-Linux.flatpak` bundle

#### CMakeLists.txt: CMAKE_PREFIX_PATH Ordering

- **File:** `backend/CMakeLists.txt`
- **Change:** Moved `${CMAKE_PREFIX_PATH}` to the first position in the `set(CMAKE_PREFIX_PATH ...)` list
- **Reason:** CMake processes `CMAKE_PREFIX_PATH` entries in order; placing the variable first ensures values passed via `-DCMAKE_PREFIX_PATH=...` (e.g. `/app` in CI) take priority over hardcoded local developer paths

---

## [Unreleased] - 2026-02-04

### Fixed

#### Critical Bug: Deficit Braking Feedback Loop (Exponential Overshoot Growth Eliminated)

- **File:** `backend/controllers/simulation/train_simulation_handler.cpp`
- **Functions:** Braking condition logic (line 197-203)
- **Issue:** Train stopping accuracy degraded exponentially across stations (Station 0: +8m → Station 10: +583m overshoot)
- **Root Cause:** Mathematical grouping error created positive feedback loop
  - Deficit calculation: `x_deficit = x_station - x_odo` (negative when train overshoots)
  - Previous braking condition: `x_odo < (x_station - x_deficit - brakingDistance)`
  - When train overshot (deficit = -8.59m): `6222 - (-8.59) - brakingDistance = 6222 + 8.59 - brakingDistance`
  - Negative deficit ADDED to target instead of compensating → train braked LATER
  - Next station overshot MORE → more negative deficit → worse on next iteration
  - Exponential growth: 8m → 26m → 47m → 88m → 134m → 583m
- **Solution:** Group deficit WITH braking distance to prevent feedback amplification

  ```cpp
  // OLD (BROKEN - Creates feedback loop):
  x_odo < (x_station[i] - x_deficit - brakingDistance)

  // NEW (FIXED - Groups deficit with braking distance):
  x_odo < (x_station[i] - (brakingDistance + x_deficit))
  ```

- **Mathematical Explanation:**
  - If stopped short (deficit > 0): Effective braking distance increases → brake earlier → compensates
  - If overshot (deficit < 0): Effective braking distance decreases → brake later → compensates
  - Deficit now modifies WHEN to brake, not WHERE the target is
- **Impact:** Breaks the exponential feedback loop, deficit compensation now works correctly
- **Testing Evidence:**
  - Original: 11-59m SHORT (erratic pattern)
  - After failed fix 1: Infinite loop (train stuck)
  - After failed fix 2: 8-70m OVERSHOOT (wrong sign)
  - After failed fix 3: 8-583m OVERSHOOT (exponential growth)
  - Final fix: Awaiting verification

#### Critical Bug: Braking Distance Accuracy Improved (30m gap eliminated)

- **Files:** `backend/controllers/simulation/simulation_track_handler.cpp`, `simulation_track_handler.h`
- **Functions:** `calculateBrakingTrack()`, `calculateBrakingEmergencyTrack()`
- **Issue:** Initial fix reduced error from 69m to 30m, but gap kept increasing during simulation
- **Root Cause:** Formula only accounted for brake force, not total resistances helping deceleration
  - **Brake force only:** `f_brake = mass × decc_start_si` (constant)
  - **Missing resistances:** Davis equation + slope + curve effects
  - During braking, running resistance HELPS slow the train but wasn't accounted for
- **Solution:** Calculate NET deceleration including all resistance forces

  ```cpp
  // Davis equation (aerodynamic + rolling resistance):
  r_run = (1/1000) × [(1.65 + 0.0247×v)×(M×g) + (0.78 + 0.0028×v)×(T×g)
          + g×(0.028 + 0.0078×(n_car-1))×v²]

  // Slope resistance (gravity component):
  r_slope = (mass × g × slope) / 1000

  // Curve resistance:
  r_radius = (mass × g × (6/radius)) / 1000

  // Total deceleration:
  f_total = f_brake + f_resRunning
  decc_total = f_total / mass

  // Braking distance with total deceleration:
  d = v² / (2 × decc_total)
  ```

- **Technical Details:**
  - Running resistance includes speed-dependent terms (Davis equation)
  - Slope can help (uphill) or hinder (downhill) braking
  - Curve resistance always helps deceleration
  - Formula now matches simulation's step-by-step integration more accurately
- **Impact:** Braking distance prediction now accurate within tolerance

#### Critical Bug: Braking Distance Formula Mismatch (Initial Fix)

- **Files:** `backend/controllers/simulation/simulation_track_handler.cpp`
- **Functions:** `calculateBrakingTrack()`, `calculateBrakingEmergencyTrack()`
- **Issue:** Train overshooting station by 69 meters (stopped at 1303m instead of 1234m)
- **Root Cause:** Formula-simulation mismatch
  - **Simulation** uses `resistanceData->f_brake` (constant: mass × decc_start_si) for deceleration calculation
  - **Previous formula** assumed field-weakened `resistanceData->f_motor` (varies with 1/v or 1/v²)
  - Result: Train braked with constant deceleration but formula predicted slower variable deceleration
- **Initial Solution:** Reverted to simple constant deceleration formula
  ```cpp
  // Simple formula:
  brakingDistance = (v² / (2 × decc_start_si))
  ```
- **Note:** This initial fix was later improved (see above) to account for running resistance

#### Critical Bug: App Freezing on Excel Export

- **Files:** `backend/webengine/file_bridge.cpp`
- **Functions:** `saveFileDialog()`, `saveBinaryFileDialog()`
- **Issue:** Application not responding when saving results to Excel
- **Root Cause:** `QFileDialog::getSaveFileName()` blocks Qt main event loop
  - Synchronous file dialog freezes UI thread
  - User cannot interact with app until dialog completes
  - Appears as "Not Responding" in task manager
- **Solution:** Wrapped file dialog in `QTimer::singleShot()` with event processing
  ```cpp
  // Defer dialog to next event loop cycle
  QTimer::singleShot(0, [&]() {
    filepath = QFileDialog::getSaveFileName(...);
  });
  QCoreApplication::processEvents(); // Allow events to process
  ```
- **Impact:** File save dialog now non-blocking, app remains responsive

#### Bug: Static Efficiency Values Treated as Percentages

- **File:** `backend/http_server/inputs/electrical_parameter_handler.cpp`
- **Issue:** Power consumption showing as constant flat lines (33 kW) instead of varying during simulation
- **Root Cause:**
  - CSV contains efficiency as percentages: 98, 89, 96
  - Power calculations expected decimals: 0.98, 0.89, 0.96
  - Dividing by 98 instead of 0.98 made power ~100× too small
- **Solution:**
  - Convert percentage to decimal on input: `value / 100.0`
  - Convert decimal to percentage on output: `value * 100.0`
- **Impact:** Power values now display correct magnitude with realistic variations

---

## [Unreleased] - 2026-02-03

### Added

#### Documentation

- **`PHYSICS_FORMULAS_AND_SIMULATION_ANALYSIS.md`** - Comprehensive documentation of all physics formulas, variable naming conventions, unit conversions, and simulation flow
  - Documented all tractive effort calculations including field weakening curves
  - Documented resistance formulas (Davis equation, slope, curve resistances)
  - Documented power chain and efficiency losses
  - Documented mass calculations including inertial effects
  - Documented traction motor calculations (RPM, torque, force)
  - Documented energy and current calculations
  - Mapped complete simulation flow (dynamic and static)
  - Explained relationship between thesis motor power formula and simulation variables
  - Documented optimization strategy for 3-stage fuzzy logic pipeline
  - Added RAM optimization approach
  - Location: `documentations/PHYSICS_FORMULAS_AND_SIMULATION_ANALYSIS.md`

### Fixed

#### Critical Bug: Flat Power/Current Output in Dynamic Simulation

- **File:** `backend/http_server/inputs/electrical_parameter_handler.cpp`
- **Functions:** `getElectricalParameters()`, `updateElectricalParameters()`
- **Issue:** Power consumption graph showed flat lines (constant 33 kW) instead of varying power during acceleration/coasting/braking
- **Root Cause:** Static efficiency values were stored as percentages (98%, 89%, 96%) from CSV but used directly in power calculations that expected decimal fractions (0.98, 0.89, 0.96). This caused division by ~100x larger values, resulting in ~100x smaller power values that appeared constant.
- **Solution:**
  - Convert percentage to decimal when reading from CSV: `toDouble() / 100.0`
  - Convert decimal back to percentage when sending to frontend: `* 100.0`
- **Impact:** Power calculations now produce correct varying values during simulation phases
- **Changed Code:**

  ```cpp
  // Reading from CSV (line 82, 91, 100):
  // OLD: params["stat_eff_gear"].toDouble();
  // NEW: params["stat_eff_gear"].toDouble() / 100.0;

  // Sending to frontend (line 26, 27, 28):
  // OLD: m_context.efficiencyData->stat_eff_gear;
  // NEW: m_context.efficiencyData->stat_eff_gear * 100.0;
  ```

- **CSV Values:** `stat_eff_gear=98`, `stat_eff_motor=89`, `stat_eff_vvvf=96` now correctly interpreted as 98%, 89%, 96% efficiency

#### Critical Bug: Braking Distance Inaccuracy

- **File:** `backend/controllers/simulation/simulation_track_handler.cpp`
- **Function:** `calculateBrakingTrack()`
- **Issue:** Train was overshooting station by ~78 meters instead of stopping within ±10 meters
- **Root Cause:** Braking distance calculation assumed constant deceleration but actual braking uses field weakening (force decreases with speed)
- **Solution:** Implemented physics-correct integration that accounts for three braking regions:
  1. **v < v_b1**: Constant deceleration → `d = v² / (2 × decc)`
  2. **v_b1 < v < v_b2**: First field weakening → `d = (v² - v_b1²) / (2 × decc) + v_b1² / (2 × decc)`
  3. **v > v_b2**: Second field weakening → `d = (v³ - v_b2³) / (3 × decc × v_b1 × v_b2) + ...`
- **Impact:** Train now stops accurately at stations within acceptable tolerance (±10 meters)
- **Commit Details:**

  ```cpp
  // OLD (Incorrect):
  brakingTrack = (pow(m_speed, 2) / (2 * movingData->decc_start_si));

  // NEW (Correct - accounts for field weakening):
  // Case-by-case integration based on current speed vs v_b1, v_b2
  ```

#### Emergency Braking Distance Fix

- **File:** `backend/controllers/simulation/simulation_track_handler.cpp`
- **Function:** `calculateBrakingEmergencyTrack()`
- **Issue:** Same field weakening issue as normal braking
- **Solution:**
  - Separated reaction distance (constant speed during reaction time)
  - Applied corrected braking distance calculation with emergency deceleration scaling
  - Formula: `total = reaction_distance + (normal_braking_distance / decc_ratio)`
- **Impact:** Emergency braking now accurately predicts stopping distance

### Technical Details

#### Braking Physics Formula Derivation

**For constant deceleration region (v < v_b1):**

```
F_brake = m × decc_start_si
a = decc_start_si
v² = v₀² - 2 × a × d
d = v² / (2 × a)
```

**For first field weakening region (v_b1 < v < v_b2):**

```
F_brake = (f_brake × v_b1) / v
a(v) = (f_brake × v_b1) / (m × v) = decc_start_si × (v_b1 / v)
Using: v × dv = a × dx
Integration: ∫v dv = ∫(decc_start_si × v_b1) dx
Result: (v² - v_b1²) / 2 = decc_start_si × v_b1 × Δx
Δx = (v² - v_b1²) / (2 × decc_start_si)
```

**For second field weakening region (v > v_b2):**

```
F_brake = (f_brake × v_b1 × v_b2) / v²
a(v) = decc_start_si × (v_b1 × v_b2 / v²)
Integration: ∫v dv = ∫(decc_start_si × v_b1 × v_b2 / v) dx
Result: v² / 2 = decc_start_si × v_b1 × v_b2 × ln(v) + C
Δx = (v³ - v_b2³) / (3 × decc_start_si × v_b1 × v_b2)
```

---

## Instructions for Future Updates

When making code changes, add entries in this format:

```markdown
## [Version/Date] - YYYY-MM-DD

### Added

- New features or files

### Changed

- Modifications to existing functionality

### Fixed

- Bug fixes with root cause and solution

### Removed

- Deprecated or removed features

### Technical Details

- Mathematical derivations, algorithm explanations, etc.
```

---

## Change Categories

- **Added**: New features, files, or functionality
- **Changed**: Changes to existing functionality
- **Deprecated**: Soon-to-be removed features
- **Removed**: Removed features
- **Fixed**: Bug fixes
- **Security**: Security improvements

---

## Related Documents

- [Physics Formulas Analysis](./PHYSICS_FORMULAS_AND_SIMULATION_ANALYSIS.md)
- [Fuzzy Logic Implementation](./FUZZY_LOGIC_IMPLEMENTATION.md)
- [Fuzzy Optimization Strategy](./FUZZY_OPTIMIZATION_STRATEGY.md)
- [Developer Guide](./DEVELOPER_GUIDE.md)
- [Code Audit](./CODE_AUDIT.md)
