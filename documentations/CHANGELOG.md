# CHANGELOG - Train Simulation App

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

<<<<<<< HEAD
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
  - **Brake force only:** `f_brake = mass × decc_start` (constant)
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
  - **Simulation** uses `resistanceData->f_brake` (constant: mass × decc_start) for deceleration calculation
  - **Previous formula** assumed field-weakened `resistanceData->f_motor` (varies with 1/v or 1/v²)
  - Result: Train braked with constant deceleration but formula predicted slower variable deceleration
- **Initial Solution:** Reverted to simple constant deceleration formula
  ```cpp
  // Simple formula:
  brakingDistance = (v² / (2 × decc_start))
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

=======
>>>>>>> 793715b (fix/simulation-and-power-bug)
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
