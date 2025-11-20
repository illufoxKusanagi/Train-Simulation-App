# Train Simulation App - Definitive Developer Guide

Welcome to the Train Simulation App development team! This guide is your definitive source for understanding the project's architecture, data flows, core features, and development workflow. It is designed to be clear and accessible, especially for new developers.

## Table of Contents

1.  [**Tech Stack**](#1-tech-stack)
2.  [**High-Level Architecture**](#2-high-level-architecture)
3.  [**Backend, Frontend, and API Communication**](#3-backend-frontend-and-api-communication)
    -   [The Communication Flow](#the-communication-flow)
    -   [API Endpoint Reference](#api-endpoint-reference)
4.  [**Data Flow: From User Input to Charted Output**](#4-data-flow-from-user-input-to-charted-output)
    -   [Static Data vs. Simulation Data](#static-data-vs-simulation-data)
    -   [End-to-End Data Trace](#end-to-end-data-trace)
5.  [**Feature Breakdown: The Simulation Engine**](#5-feature-breakdown-the-simulation-engine)
    -   [How the Simulation Works](#how-the-simulation-works)
    -   [Core Simulation Handlers](#core-simulation-handlers)
6.  [**File Handling: Saving and Exporting**](#6-file-handling-saving-and-exporting)
    -   [How to Save Simulation Results (CSV)](#how-to-save-simulation-results-csv)
    -   [Future Feature: How to Save to Excel](#future-feature-how-to-save-to-excel)
    -   [Future Feature: How to Save Charts as Images](#future-feature-how-to-save-charts-as-images)
7.  [**How to Build and Develop**](#7-how-to-build-and-develop)
    -   [Prerequisites](#prerequisites)
    -   [Development Workflow](#development-workflow)
8.  [**Project Structure**](#8-project-structure)

---

## 1. Tech Stack

This project combines a high-performance C++ backend with a modern web frontend.

-   **Backend**:
    -   **Language**: C++17
    -   **Framework**: Qt 6
    -   **Core Modules**: `QtCore`, `QtNetwork` (for the HTTP server), `QJsonDocument` (for handling JSON), `QtWebEngine` and `QtWebChannel` for desktop app integration.
    -   **Build System**: CMake with Ninja for fast builds.

-   **Frontend**:
    -   **Framework**: Next.js 14 (with React 18)
    -   **Language**: TypeScript
    -   **UI Components**: Shadcn UI and Radix UI for accessible and reusable components.
    -   **Styling**: Tailwind CSS for utility-first styling.
    -   **Data Fetching**: TanStack Query (React Query) for managing server state.
    -   **Charts**: Recharts for data visualization.
    -   **HTTP Requests**: Axios for making API calls to the backend.

-   **Desktop App Shell**:
    -   **Technology**: Qt WebEngine
    -   **Function**: Wraps the Next.js frontend into a native desktop window, providing a seamless desktop application experience. The `FileBridge` class (`webengine/file_bridge.h`) exposes native file dialogs to the frontend via Qt's WebChannel.

## 2. High-Level Architecture

The application operates in two main parts:

1.  **The C++ Backend**: A powerful command-line application that runs a lightweight HTTP server. It exposes a REST API to receive input data, run complex physics simulations, and serve the results.
2.  **The Next.js Frontend**: A modern web application that provides the user interface. Users interact with forms, buttons, and charts in the frontend.

These two parts are bridged by the **Qt WebEngine**, which essentially acts as a browser, displaying the frontend and allowing it to communicate with the backend server running locally.

## 3. Backend, Frontend, and API Communication

### The Communication Flow

The frontend and backend communicate via a **local REST API**. The C++ backend runs an HTTP server on `localhost:8080`, and the Next.js frontend sends HTTP requests to this server.

```
+--------------------------------+         +--------------------------------+
|      Frontend (Next.js)        |         |        Backend (C++)           |
| (Runs on http://localhost:3000)|         | (Runs on http://localhost:8080)|
+--------------------------------+         +--------------------------------+
|                                |         |                                |
| 1. User enters data into a form|         |                                |
|    (e.g., Train Mass)          |         |                                |
|                                |-------->| 2. Frontend sends a POST       |
| 4. Receives JSON data,         |         |    request with JSON body:     |
|    updates charts with         |         |    /api/parameters/train       |
|    TanStack Query & Recharts   |<--------|                                |
|                                |         | 3. Backend's HttpServer &      |
|                                |         |    ApiHandler parse the JSON,  |
|                                |         |    update the simulation state,|
|                                |         |    and send a success response.|
+--------------------------------+         +--------------------------------+
```

-   **Data Format**: All data is exchanged in **JSON** format.
-   **Backend API Logic**: Located in `backend/http_server/api_handler.cpp`. This file is the central router that maps incoming HTTP requests to specific C++ functions.
-   **Frontend API Calls**: Located in `frontend/src/services/api.ts`. These functions use `axios` to abstract the HTTP requests, making it easy to call backend endpoints from the UI.

### API Endpoint Reference

| Method | Endpoint                  | Description                                   | Request Body (JSON Example)      | Backend Handler File                 |
| :----- | :------------------------ | :-------------------------------------------- | :------------------------------- | :----------------------------------- |
| `POST` | `/api/parameters/train`   | Sets the static parameters for the train.     | `{ "mass": 100, "cars": 8 }`     | `train_parameter_handler.cpp`        |
| `POST` | `/api/parameters/running` | Sets the static parameters for the run.       | `{ "maxSpeed": 120 }`            | `running_parameter_handler.cpp`      |
| `POST` | `/api/parameters/electrical`| Sets the static electrical parameters.      | `{ "voltage": 750 }`             | `electrical_parameter_handler.cpp`   |
| `POST` | `/api/parameters/track`   | Sets the static parameters for the track.     | `{ "length": 10000, "grade": 1 }`| `track_parameter_handler.cpp`        |
| `POST` | `/api/simulation/start`   | Tells the backend to start the simulation.    | `{ "type": "static" }`           | `simulation_handler.cpp`             |
| `GET`  | `/api/simulation/results` | Fetches the simulation output after it's done.| (empty)                          | `simulation_handler.cpp`             |
| `POST` | `/api/export/csv`         | Asks the backend to generate and save a CSV.  | `{ "variables": ["speed"] }`     | `csv_output_handler.cpp`             |
| `POST` | `/api/init/quick`         | Initializes the backend with default values.  | (empty)                          | `api_handler.cpp`                    |

## 4. Data Flow: From User Input to Charted Output

### Static Data vs. Simulation Data

Understanding the two types of data is crucial.

-   **Static Data (The "Inputs")**:
    -   **What it is**: The configuration parameters that define the conditions for a simulation. This data is set by the user and **does not change** during a simulation run.
    -   **Examples**: Train mass, track length, motor efficiency, maximum speed.
    -   **Where it lives**:
        -   **Frontend**: Managed by React state (`useState`) within various form components.
        -   **Backend**: Sent via the API and stored in C++ data structures (e.g., `TrainData`, `TrackData`) inside the `AppContext`. These structs are defined in `/backend/models/`.

-   **Simulation Data (The "Outputs")**:
    -   **What it is**: The time-series results generated by the simulation engine. It represents the state of the train at each moment in time.
    -   **Examples**: The train's speed, distance, and power consumption at t=0s, t=1s, t=2s, etc.
    -   **Where it lives**:
        -   **Backend**: Generated and stored in a `std::vector<SimulationData>` within the `AppContext`.
        -   **Frontend**: Fetched from the backend via the `/api/simulation/results` endpoint and managed by **TanStack Query**. This cached data is then passed to `Recharts` to render the graphs.

### End-to-End Data Trace

Here’s what happens when a user provides input and clicks "Start Simulation":

1.  **Input (Frontend)**: A user types a value (e.g., `10000` kg) into an input field in a React component. This updates the component's state. When the user is done, the data is sent to the backend via a `POST` request to the appropriate `/api/parameters/*` endpoint.
2.  **Static Data Storage (Backend)**: The `ApiHandler` in the C++ backend receives the request, parses the JSON, and calls the relevant data handler (e.g., `TrainDataHandler`). This handler updates the corresponding data model (e.g., `TrainData`) inside the `AppContext`, which is our central data store on the backend.
3.  **Simulation Start (Frontend)**: The user clicks the "Start Simulation" button. This triggers a `POST` request to `/api/simulation/start`.
4.  **Simulation Execution (Backend)**: The `ApiHandler` routes this request to `SimulationHandler::startSimulation()`. This function kicks off the main simulation loop, which calculates the train's physics for each time step and stores the results in the `AppContext`.
5.  **Results Fetching (Frontend)**: After the simulation is complete, the frontend automatically sends a `GET` request to `/api/simulation/results`.
6.  **Results Formatting (Backend)**: The `ApiHandler` gets the vector of `SimulationData` from the `AppContext`, converts it into a large JSON array, and sends it back in the HTTP response.
7.  **Display (Frontend)**: TanStack Query receives the JSON data, caches it, and makes it available to the UI. React detects the new data and re-renders the `Recharts` components to display the simulation results as charts.

## 5. Feature Breakdown: The Simulation Engine

### How the Simulation Works

The simulation is a **discrete time-step physics loop**. It calculates the state of the train at `t=0`, then uses those results to calculate the state at `t=1`, and so on, until the train reaches the end of the track.

The core logic resides in `backend/controllers/simulation/`. The main `TrainSimulationHandler` orchestrates a series of specialized handlers.

### Core Simulation Handlers

Each handler is a C++ class responsible for a specific piece of the physics calculation.

-   **`MassHandler`**:
    -   **Purpose**: Calculates the total *effective* mass of the train.
    -   **How it's used**: This is more than just the static weight. It includes the rotational inertia of the wheels and motors, which adds an "equivalent mass" that must be accelerated. This is a crucial first step for all force calculations.

-   **`ResistanceHandler`**:
    -   **Purpose**: Calculates all forces that oppose the train's motion.
    -   **How it's used**: It sums up several types of resistance:
        -   **Rolling Resistance**: Friction from the wheels on the track.
        -   **Aerodynamic Drag**: Air resistance, which grows exponentially with speed.
        -   **Gradient Resistance**: The force of gravity pulling the train backward on an incline (or pushing it forward on a decline).

-   **`TractiveEffortHandler`**:
    -   **Purpose**: Determines the forward force generated by the motors.
    -   **How it's used**: It calculates the maximum possible propulsion force based on the motor's characteristics and the current speed, ensuring this force doesn't exceed the limits of wheel-on-rail adhesion (to prevent wheel slip).

-   **`TractionMotorHandler`**:
    -   **Purpose**: Models the performance and limits of the train's motors.
    -   **How it's used**: Simulates how the motors convert electrical power into mechanical force, considering the motor's power curve, torque-speed characteristics, and efficiency.

-   **`PowerHandler` & `EnergyHandler`**:
    -   **Purpose**: Calculate the power required and total energy consumed.
    -   **How it's used**: `PowerHandler` calculates the instantaneous power needed to overcome resistance and accelerate. `EnergyHandler` integrates this power over time to track total energy consumption (in kWh).

-   **`MovingHandler`**:
    -   **Purpose**: Calculates the train's acceleration, velocity, and position.
    -   **How it's used**: This is the heart of the physics loop. It uses Newton's second law (F=ma):
        1.  Calculates **Net Force** = Tractive Effort - Total Resistance.
        2.  Calculates **Acceleration** = Net Force / Total Effective Mass.
        3.  Updates **Velocity** based on the acceleration over the time step.
        4.  Updates **Position** based on the new velocity.

## 6. File Handling: Saving and Exporting

### How to Save Simulation Results (CSV)

-   **Relevant Code**: `backend/controllers/output/csv_output_handler.cpp` and `backend/webengine/file_bridge.cpp`
-   **Process**:
    1.  **Trigger**: The user clicks an "Export to CSV" button on the frontend.
    2.  **Frontend Logic**: The frontend calls a function that uses the `Qt WebChannel` to communicate with the C++ backend. Specifically, it calls the `saveFileDialog` method on the `fileBridge` object.
    3.  **Backend Logic**: The `fileBridge` in the C++ backend receives the request, opens a native file save dialog, and upon user confirmation, writes the provided data to the selected file.
    4.  **Data Generation**: The `CsvOutputHandler` is responsible for generating the CSV-formatted string from the simulation results stored in `AppContext`.

### Future Feature: How to Save to Excel

This feature is not yet implemented. It would require a C++ library capable of writing `.xlsx` files.

-   **Proposed Library**: **OpenXLSX** or **libxlsxwriter**.
-   **Implementation Steps**:
    1.  Add the chosen library to the `CMakeLists.txt` and link it.
    2.  Create a new `ExcelOutputHandler` class, similar to `CsvOutputHandler`.
    3.  This handler would use the library's API to create a workbook, write the headers, and write the data row by row.
    4.  Add a new API endpoint (e.g., `/api/export/excel`) to trigger this handler.

### Future Feature: How to Save Charts as Images

This feature should be implemented entirely on the **frontend**.

-   **Proposed Library**: **html-to-image**. This library can convert any DOM element into a PNG or JPG.
-   **Implementation Steps**:
    1.  Install the library: `npm install html-to-image`.
    2.  In the React component containing the chart, get a `ref` to the chart's parent `<div>`.
    3.  Add a "Save as PNG" button with an `onClick` handler.
    4.  The handler function would look like this:
        ```typescript
        import { toPng } from 'html-to-image';

        const exportChartAsPng = async (chartRef: React.RefObject<HTMLDivElement>) => {
          if (chartRef.current === null) return;

          const dataUrl = await toPng(chartRef.current);

          // Create a link to trigger the download
          const link = document.createElement('a');
          link.download = 'simulation-chart.png';
          link.href = dataUrl;
          link.click();
        };
        ```

## 7. How to Build and Develop

### Prerequisites
-   **Node.js**: v18 or later.
-   **C++ Compiler**: GCC (Linux), Clang (macOS), or MSVC (Windows).
-   **Qt SDK**: Version 6.5 or later (with `WebEngine` and `HttpServer` modules).
-   **Build tools**: CMake (v3.16+) and Ninja.

### Development Workflow

For active development, you need to run the backend and frontend servers simultaneously.

1.  **Terminal 1: Run the Backend**
    -   **First-time setup**: `cmake -B backend/build -S backend -G Ninja`
    -   **Compile**: `cmake --build backend/build` (re-run this whenever you change C++ files).
    -   **Run**: `./backend/build/bin/TrainSimulationApp --headless`
    -   The `--headless` flag tells the backend to only run the HTTP server without launching the Qt WebEngine window.

2.  **Terminal 2: Run the Frontend**
    -   **Install dependencies**: `npm install`
    -   **Run**: `npm run dev:frontend`
    -   This starts the Next.js development server on `http://localhost:3000`.

3.  **View the App**
    -   Open `http://localhost:3000` in your web browser. The frontend will connect to the backend server automatically.

## 8. Project Structure

```
/
├── 📁 backend/          # All C++ Backend Code
│   ├── 📁 controllers/  # The "brains" - contains all simulation logic
│   │   ├── 📁 data/     # Handlers for incoming static data
│   │   ├── 📁 output/   # CsvOutputHandler lives here
│   │   └── 📁 simulation/ # Core physics handlers (Mass, Resistance, etc.)
│   ├── 📁 core/        # AppContext, the central C++ data store
│   ├── 📁 http_server/ # API server and endpoint routing
│   ├── 📁 models/      # C++ data structures (TrainData, SimulationData)
│   ├── 📁 webengine/   # The desktop window that hosts the UI
│   ├── main.cpp       # Entry point for the C++ application
│   └── CMakeLists.txt # Build instructions for the backend
│
├── 📁 frontend/        # All Next.js/React Frontend Code
│   └── 📁 src/
│       ├── 📁 app/      # Application pages and layouts
│       ├── 📁 components/ # Reusable UI elements (buttons, forms, charts)
│       ├── 📁 services/ # Functions for making API calls (axios wrappers)
│       └── 📁 types/    # TypeScript type definitions
│
└── 📄 package.json      # Defines Node.js dependencies and scripts
```
