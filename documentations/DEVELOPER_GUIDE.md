# Developer Guide

This guide provides a comprehensive overview of the Train Simulation App's architecture, data flow, and calculation logic. It is designed to help developers understand how the frontend and backend interact and how the simulation engine works.

## 1. Project Structure

The project is a hybrid application combining a **Next.js frontend** with a **Qt C++ backend**.

### Directory Overview

- **`frontend/`**: Contains the Next.js application.
  - `src/app/`: Next.js App Router pages (UI).
  - `src/components/`: React components (charts, forms, UI elements).
  - `src/services/`: API client (`api.ts`) for communicating with the backend.
  - `src/lib/qt-webchannel.ts`: Bridge for native file dialogs.
  - `src/types/`: TypeScript type definitions matching backend data structures.

- **`backend/`**: Contains the Qt C++ application.
  - `http_server/`: Handles HTTP requests from the frontend.
    - `http_server.cpp`: Sets up the server on port 8080.
    - `api_handler.cpp`: Routes requests to specific controllers.
  - `controllers/`: Business logic and simulation engine.
    - `simulation/`: Core simulation logic (`TrainSimulationHandler`).
    - `data/`: Handlers for specific parameter types (Mass, Power, etc.).
  - `models/`: Data structures (`struct`s) holding simulation state.
    - `simulation_data.h`: Stores time-series results.
    - `train_data.h`, `mass_data.h`, etc.: Input parameters.
  - `webengine/`: Qt WebEngine setup to host the frontend.
    - `webengine_window.cpp`: Main window container.
    - `file_bridge.cpp`: Native file saving logic.

## 2. Data Flow & Communication

The application uses a **Client-Server** architecture, even when running locally as a desktop app.

### Communication Loop

1.  **User Interaction (Frontend)**:
    - User updates parameters or clicks "Start Simulation" in the React UI.
    - `frontend/src/services/api.ts` sends an HTTP request (GET/POST) to `http://localhost:8080/api/...`.

2.  **Request Handling (Backend)**:
    - `HttpServer` receives the request.
    - `ApiHandler` routes it based on the URL (e.g., `/api/simulation/start` -> `m_simulationHandler`).

3.  **Processing (Backend)**:
    - Controllers update the data models in `AppContext`.
    - For simulation, `TrainSimulationHandler` runs the physics loop.

4.  **Response (Backend)**:
    - Controllers return a `QJsonObject`.
    - `HttpServer` sends it back as a JSON response.

5.  **UI Update (Frontend)**:
    - React receives the JSON data.
    - State is updated, and charts/tables are re-rendered.

### HTTP Transaction Flow

The `HttpServer` in `backend/http_server/http_server.cpp` manages the transaction:

1.  **Listening**: Starts a TCP server on port 8080.
2.  **Routing**: `setupRoutes()` maps paths (e.g., `/api/parameters/train`) to lambda functions.
3.  **CORS**: Middleware adds Access-Control headers to allow requests from the frontend (running on port 3000 in dev).
4.  **Parsing**: `parseRequestBody()` converts the raw HTTP body into a `QJsonObject`.
5.  **Dispatch**: Calls the appropriate method in `ApiHandler` (e.g., `handleUpdateTrainParameters`).
6.  **Response**: Wraps the result in a `QHttpServerResponse` with status 200 OK (or error codes).

### Native File Bridge (WebChannel)

For saving files (CSV/Excel) directly to the user's disk with native dialogs, we use `Qt WebChannel`.

1.  **Backend Setup (`webengine_window.cpp`)**:
    - Creates a `QWebChannel` and registers a `FileBridge` object.
    - `FileBridge` (`file_bridge.cpp`) exposes `saveFileDialog` and `saveBinaryFileDialog` slots.

2.  **Frontend Setup (`qt-webchannel.ts`)**:
    - Loads `qwebchannel.js`.
    - Connects to `window.qt.webChannelTransport`.
    - Exposes `window.fileBridge` to the React app.

3.  **Usage**:
    - Frontend calls `saveFileWithDialog(data, filename)`.
    - This invokes the C++ `FileBridge::saveFileDialog`.
    - C++ opens a native `QFileDialog`.
    - File is saved by C++ directly to the chosen path.

## 3. API Documentation

### Parameters

| Endpoint | Method | Description | Payload |
|----------|--------|-------------|---------|
| `/api/parameters/train` | GET | Get train constants | - |
| `/api/parameters/train` | POST | Update train constants | `{ trainParameters: { ... } }` |
| `/api/parameters/electrical` | GET | Get electrical params | - |
| `/api/parameters/electrical` | POST | Update electrical params | `{ electricalParameters: { ... } }` |
| `/api/parameters/running` | GET | Get running params | - |
| `/api/parameters/running` | POST | Update running params | `{ runningParameters: { ... } }` |
| `/api/parameters/track` | GET | Get track params | - |
| `/api/parameters/track` | POST | Update track params | `{ trackParameters: { ... } }` |
| `/api/parameters/mass` | GET | Get mass params | - |
| `/api/parameters/mass` | POST | Update mass params | `{ massParameters: { ... } }` |

### Simulation

| Endpoint | Method | Description | Payload |
|----------|--------|-------------|---------|
| `/api/simulation/start` | POST | Start simulation | `{ type: "static" }` |
| `/api/simulation/status` | GET | Check if running | - |
| `/api/simulation/results` | GET | Get full results | - |

### System

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/health` | GET | Check backend status |
| `/api/init/quick` | POST | Initialize default values |
| `/api/debug/context` | GET | Dump current AppContext state |

## 4. Data Models Guide

Based on `legends.md`, here is how variables are named and structured in the backend models.

### Prefixes
- `n_`: Count (e.g., `n_M1` = Number of Motor cars type 1)
- `mass_`: Mass in kg (e.g., `mass_M1`)
- `f_`: Force in kN (e.g., `f_resRunning`)
- `v_`: Speed in km/h
- `p_`: Power in kW
- `e_`: Energy in kWh
- `eff_`: Efficiency % (0.0 - 1.0)

### Key Models (`AppContext`)

The `AppContext` class is the single source of truth. It holds pointers to:

- **`TrainData`**: Static train properties (length, wheel diameter, gear ratio).
- **`MassData`**: Mass of cars, passengers, and rotational inertia.
- **`RunningData`**: Dynamic state (current speed `v`, acceleration `acc`, distance `x`).
- **`PowerData`**: Instantaneous power values (`p_wheel`, `p_catenary`).
- **`EnergyData`**: Cumulative energy (`e_motor`, `e_regen`).
- **`SimulationDatas`**: Arrays storing the history of all variables for plotting.

## 5. Calculation Logic

The core physics engine resides in `backend/controllers/simulation/train_simulation_handler.cpp`.

### Simulation Loop

1.  **Initialization**: Loads parameters and sets $t=0, v=0, x=0$.
2.  **Step Calculation**:
    - **Resistance**: $R = A + Bv + Cv^2 + R_{grade} + R_{curve}$
    - **Tractive Effort**: $F_{motor}$ calculated from throttle/brake notch curves.
    - **Net Force**: $F_{net} = F_{motor} - R$
    - **Acceleration**: $a = F_{net} / M_{effective}$ (where $M_{effective}$ includes rotational inertia).
    - **Integration**:
        - $v_{t+1} = v_t + a \cdot \Delta t$
        - $x_{t+1} = x_t + v_t \cdot \Delta t$
3.  **Energy**: $E += P \cdot \Delta t$
4.  **Recording**: Push current values to `SimulationDatas`.

## 6. Frontend Implementation

The frontend is a **Next.js 14+** application using the App Router.

### Key Pages (`src/app/`)

- **`page.tsx`**: Dashboard and landing page.
- **`train-parameter/page.tsx`**: Form for editing `TrainData`.
- **`simulation/page.tsx`**: Controls for starting/stopping simulation.
- **`results/page.tsx`**: Charts and tables displaying `SimulationDatas`.

### The "Bridge" Concept

The frontend does not calculate physics. It acts as a **View** and **Controller** for the C++ **Model**.

- **Input**: React forms -> API POST -> C++ Models.
- **Output**: C++ Models -> API GET -> React Charts.
- **File IO**: React Button -> WebChannel -> C++ FileDialog.

This separation allows the heavy physics calculations to run in optimized C++ while providing a modern, responsive UI in React.
