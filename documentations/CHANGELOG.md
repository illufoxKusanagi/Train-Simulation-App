# CHANGELOG - Train Simulation App

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

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
  brakingTrack = (pow(m_speed, 2) / (2 * movingData->decc_start));
  
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
F_brake = m × decc_start
a = decc_start
v² = v₀² - 2 × a × d
d = v² / (2 × a)
```

**For first field weakening region (v_b1 < v < v_b2):**
```
F_brake = (f_brake × v_b1) / v
a(v) = (f_brake × v_b1) / (m × v) = decc_start × (v_b1 / v)
Using: v × dv = a × dx
Integration: ∫v dv = ∫(decc_start × v_b1) dx
Result: (v² - v_b1²) / 2 = decc_start × v_b1 × Δx
Δx = (v² - v_b1²) / (2 × decc_start)
```

**For second field weakening region (v > v_b2):**
```
F_brake = (f_brake × v_b1 × v_b2) / v²
a(v) = decc_start × (v_b1 × v_b2 / v²)
Integration: ∫v dv = ∫(decc_start × v_b1 × v_b2 / v) dx
Result: v² / 2 = decc_start × v_b1 × v_b2 × ln(v) + C
Δx = (v³ - v_b2³) / (3 × decc_start × v_b1 × v_b2)
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
