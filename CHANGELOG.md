# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Fixed

- **Excel Export Freeze**: Fixed blocking file dialog that froze the UI during Excel export
  - Wrapped `QFileDialog::getSaveFileName()` in `QTimer::singleShot()` for non-blocking execution
  - Added `QCoreApplication::processEvents()` to keep UI responsive
  - Location: `backend/webengine/file_bridge.cpp`

- **Efficiency Percentage Conversion**: Fixed efficiency parameter scaling issue
  - CSV input values (e.g., 98%) now correctly divided by 100 on load (→ 0.98)
  - Output multiplied by 100 for display consistency
  - Impact: Power calculations now use correct efficiency values (0.98 instead of 98)
  - Location: `backend/http_server/inputs/electrical_parameter_handler.cpp`

- **Braking Distance Calculation**: Implemented numerical integration for field weakening braking
  - Replaced analytical approximations with timestep-by-timestep integration
  - Now accounts for field weakening regions (constant, ∝1/v, ∝1/v²) accurately
  - Uses kinematic equation: d = v×dt + 0.5×a×dt² matching simulation exactly
  - Location: `backend/controllers/simulation/simulation_track_handler.cpp`

### Added

- **Distance Deficit Compensation**: Implemented carry-over system for braking accuracy errors
  - Added `distanceDeficit` field to `StationData` model
  - Calculates deficit when train stops: `deficit = targetStop - actualStop`
  - Applies deficit to next segment braking target: `odo < (station + distanceDeficit)`
  - Simple compensation approach suggested by user to maintain total distance accuracy
  - Location: `backend/models/station_data.h`, `backend/controllers/simulation/train_simulation_handler.cpp`

### Known Issues

⚠️ **Progressive Distance Gap**: Despite deficit compensation implementation, simulation still encounters a distance gap that grows larger at each station. The deficit calculation or application may not be working correctly. Further investigation needed.
  - Station 1: ~12m short (stops at 1222m vs target 1234m)
  - Station 3: ~32m short (stops at 14697m vs target 14729m)
  - Gap increases progressively, suggesting cumulative error or deficit not being applied
  - Root cause: Under investigation

### Notes

- All changes are backend-only, no frontend modifications required
- Distance deficit approach prioritizes simplicity over perfect physics modeling
- Field weakening braking integration matches simulation physics exactly

## [Previous Versions]

(To be added when version releases are created)
