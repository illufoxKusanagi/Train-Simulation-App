# Code Audit Report

This document lists vulnerabilities, potential errors, inconsistencies, ambiguities, and bugs identified during the code audit of the Train Simulation App.

## 1. Vulnerabilities & Critical Bugs

### [CRITICAL] Potential Server Crash in Simulation Handler
**Location**: `backend/controllers/simulation/train_simulation_handler.cpp`
**Issue**: The handler accesses `stationData` and other model pointers directly without checking if they are valid (non-null).
**Code Snippet**:
```cpp
void TrainSimulationHandler::calculateRunningResEachSlope() {
  if (stationData->stat_slope_1) { // CRASH if stationData is null
    // ...
  }
}
```
**Risk**: If `startSimulation` is called before `stationData` is initialized (e.g., via `quickInit` or full parameter update), the backend will crash with a segmentation fault.
**Solution**: Add null checks at the beginning of `startStaticSimulation` or within helper methods.
```cpp
if (!stationData) {
    qCritical() << "Station data not initialized!";
    return;
}
```

### [HIGH] Logic Duplication (Frontend vs Backend)
**Location**: `frontend/src/app/train-parameter/page.tsx` vs `backend/models/mass_data.h` (implied logic)
**Issue**: The frontend calculates `mass_totalEmpty`, `mass_totalLoad`, and `mass_totalInertial` in `recalculateMass()` using JavaScript. The backend likely performs similar calculations.
**Risk**: If the formula changes in the backend (e.g., how inertial mass is calculated), the frontend will show incorrect "preview" values, leading to user confusion.
**Solution**:
1.  **Short term**: Add comments in both files referencing the other.
2.  **Long term**: Create an API endpoint `/api/calculate/mass` so the frontend sends inputs and gets the canonical calculated values from the backend.

## 2. Potential Errors & Code Quality

### [MEDIUM] Hardcoded "Magic Numbers"
**Location**: `backend/http_server/api_handler.cpp` (`handleQuickInit`)
**Issue**: Default values for simulation parameters are hardcoded in the C++ source code.
```cpp
m_context.trainData->n_tm = 24.0;
m_context.trainData->wheel = 860.0;
```
**Risk**: Hard to maintain. If defaults need to change, recompilation is required.
**Solution**: Move default values to a `config.json` file or a dedicated `ConstantDefinitions` header/class.

### [LOW] Raw Pointer Usage
**Location**: `backend/http_server/api_handler.cpp`
**Issue**: Uses `new` to allocate handlers without smart pointers.
```cpp
m_trainHandler = new TrainParameterHandler(context, this);
```
**Risk**: While `QObject` parent-child system handles deletion, using `std::unique_ptr` or `std::shared_ptr` is safer and more modern C++.
**Solution**: Refactor to use `std::unique_ptr` or `QScopedPointer` where appropriate.

### [LOW] Frontend Error Handling via `alert()`
**Location**: `frontend/src/app/page.tsx`
**Issue**: Uses `alert("Simulation failed...")` for error reporting.
**Risk**: Poor user experience. Alerts block the UI and look unprofessional.
**Solution**: Use the `toast` component (already used in `train-parameter/page.tsx`) for consistent error feedback.

## 3. Inconsistencies & Ambiguities

### [LOW] Variable Naming
**Location**: `legends.md` vs Code
**Issue**: `legends.md` defines `n_` as "Number/quantity", but some code uses `numberOf...` in JSON keys (e.g., `numberOfMotorCars` in `train-parameter/page.tsx`).
**Risk**: Confusion when mapping frontend forms to backend models.
**Solution**: Standardize on the short names (`n_tm`, `n_car`) or the long names everywhere. The current mix requires mental mapping.

### [LOW] Type Casting in Output
**Location**: `frontend/src/app/output/page.tsx`
**Issue**: Uses `as Record<string, unknown>` casting for CSV generation.
**Risk**: If the API response structure changes, the CSV export might break silently or produce `[object Object]`.
**Solution**: Define a strict TypeScript interface for the simulation results row and use it instead of `unknown`.

## 4. Deep Audit Findings (Advanced)

### [CRITICAL] Blocking Simulation Loop (Threading)
**Location**: `backend/controllers/simulation/train_simulation_handler.cpp`
**Issue**: The `startStaticSimulation` method runs a `while` loop on the main thread.
```cpp
while (movingData->x_total < stationData->stat_x_station) { ... }
```
**Risk**: This blocks the entire backend server. During a long simulation, the API will not respond to `/api/simulation/status` or `/api/health`. The frontend polling will time out.
**Solution**: Move the simulation loop to a separate `QThread` or use `QtConcurrent::run`.

### [HIGH] Missing Input Range Validation
**Location**: `backend/http_server/inputs/train_parameter_handler.cpp`
**Issue**: Parameters are assigned directly without range checks.
```cpp
m_context.trainData->n_tm = trainParams["tractionMotors"].toDouble();
```
**Risk**: A user could send negative numbers or zero for critical values (e.g., `n_car = -5`, `mass = 0`), leading to physical impossibilities or division-by-zero crashes in the physics engine.
**Solution**: Add a validation layer.
```cpp
double n_tm = trainParams["tractionMotors"].toDouble();
if (n_tm < 0) throw std::invalid_argument("Traction motors cannot be negative");
```

### [MEDIUM] Unbounded Memory Growth
**Location**: `backend/models/simulation_data.h`
**Issue**: `QList<double>` vectors grow indefinitely during simulation.
**Risk**: For very long simulations (e.g., hours) or high time resolution (small `dt`), the application could run out of memory.
**Solution**: Implement a circular buffer or write results to disk periodically instead of keeping everything in RAM.

## 5. Recommended Next Steps

1.  **Fix the Critical Crash**: Immediately add null pointer checks to `TrainSimulationHandler`.
2.  **Refactor Frontend Logic**: Move mass calculation to the backend or strictly synchronize the formulas.
3.  **Improve Error UX**: Replace `alert()` with `toast()` in the main dashboard.
4.  **Thread the Simulation**: Move the physics loop to a worker thread to prevent server freezing.
5.  **Validate Inputs**: Add range checks in all parameter handlers.
