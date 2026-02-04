# Physics Formulas and Simulation Analysis

## Document Purpose
This document provides a comprehensive breakdown of all physics formulas used in the Train Simulation App, variable mappings, unit conversions, and the complete data flow through the simulation engine. This analysis is essential for implementing the 3-stage fuzzy logic optimization pipeline.

**Analysis Date:** February 3, 2026  
**Analyzed Code:** `backend/controllers_copy/simulation/` (original implementation)  
**Reference:** `legends.md` for variable naming conventions

---

## Table of Contents
1. [Variable Naming Conventions](#1-variable-naming-conventions)
2. [Unit Conversions](#2-unit-conversions)
3. [Tractive Effort Formulas](#3-tractive-effort-formulas)
4. [Resistance Formulas](#4-resistance-formulas)
5. [Power Calculations](#5-power-calculations)
6. [Mass Calculations](#6-mass-calculations)
7. [Traction Motor Calculations](#7-traction-motor-calculations)
8. [Energy Calculations](#8-energy-calculations)
9. [Current Calculations](#9-current-calculations)
10. [Simulation Flow](#10-simulation-flow)
11. [Motor Power vs Simulation Variables](#11-motor-power-vs-simulation-variables)
12. [Optimization Strategy](#12-optimization-strategy)

---

## 1. Variable Naming Conventions

### Prefixes
- `n_` : Number/quantity (e.g., `n_tm` = number of traction motors)
- `mass_` : Mass value in kg/tons
- `r_` : Resistance value in kN
- `f_` : Force value in kN
- `v_` : Velocity/speed value in km/h
- `tm_` : Per traction motor value
- `x_` : Distance value in m
- `p_` : Power value in kW
- `e_` : Energy value in kWh
- `eff_` : Efficiency value (0.0-1.0 or %)
- `vol_` : Voltage in V
- `curr_` : Current in A
- `stat_` : Static simulation variables

### Suffixes
- `_si` : SI unit version (m/s² for acceleration, m/s for velocity)
- `_M` : Motor car related
- `_T` : Trailer car related
- `_p` : Powering phase related
- `_b` : Braking phase related
- `_res` : Resistance related
- `_i` : Inertial related

### Key Composite Variables
- `mass_totalEmpty` : Total empty train mass (tons)
- `mass_totalLoad` : Total loaded train mass (tons)
- `mass_totalInertial` : Total inertial mass including rotational effects (tons → kg in calculations)
- `acc_start` : Starting acceleration capability (m/s²)
- `v_p1`, `v_p2` : First and second field weakening points (km/h)
- `v_b1`, `v_b2` : Braking weakening points (km/h)
- `f_start` : Starting tractive effort (kN)
- `f_motor` : Current motor force (kN)
- `f_total` : Net force (motor force - resistance) (kN)

---

## 2. Unit Conversions

### Constants (from `constant_data.h`)
```cpp
g = 9.8;          // Gravitational acceleration (m/s²)
cV = 3.6;         // Conversion: m/s to km/h (or km/h to m/s when dividing)
cF = 1000/9.8;    // Conversion: kN to kgf
dt = 0.1;         // Time step (seconds)
```

### Common Conversions in Code
```cpp
// Velocity
v_si = v / cV;           // km/h → m/s
v = v_si * cV;           // m/s → km/h

// Acceleration
acc_si = acc / cV;       // km/h/s → m/s²
acc = acc_si * cV;       // m/s² → km/h/s

// Power at wheel (Note: v is in km/h, needs conversion to m/s)
p_wheel = f_motor * (v / cV);  // kN × m/s = kW
```

**IMPORTANT:** 
- `acc_start` is in **m/s²** (input parameter)
- `acc` is in **km/h/s** (simulation variable)
- `acc_si` is in **m/s²** (SI unit for calculations)

---

## 3. Tractive Effort Formulas

**Source:** `backend/controllers_copy/simulation/tractive_effort_handler.cpp`

### 3.1 Starting Force Calculation
```cpp
f_start = mass_totalInertial × acc_start + f_resStart
```

**Where:**
- `mass_totalInertial` : Total inertial mass (tons, converted to kg in practice)
- `acc_start` : Starting acceleration capability (m/s²)
- `f_resStart` : Starting resistance force (kN)
- `f_start` : Maximum starting tractive effort (kN)

**Code Location:** `tractive_effort_handler.cpp` line 12-14

---

### 3.2 Motor Force vs Speed Characteristic (Field Weakening)

**This is the CUSTOM FORMULA representing DC/AC motor behavior:**

#### Phase 1: Constant Torque Region (v ≤ v_p1)
```cpp
f_motor = f_start  // Constant force
```

**Physics:** Motor operates at maximum torque/current limit.

#### Phase 2: First Field Weakening (v_p1 < v ≤ v_p2)
```cpp
f_motor = (f_start × v_p1) / v  // Hyperbolic decay
```

**Physics:** Constant power region. As speed increases, force decreases to maintain constant power.

**Power:** `P = f_start × v_p1 = constant`

#### Phase 3: Second Field Weakening (v > v_p2)
```cpp
f_motor = (f_start × v_p1 × v_p2) / (v²)  // Quadratic decay
```

**Physics:** Voltage-limited region. Force drops with square of velocity.

**Power:** `P = (f_start × v_p1 × v_p2) / v` (decreasing power)

**Code Location:** `tractive_effort_handler.cpp` lines 28-42

---

### 3.3 Total Force Calculation
```cpp
f_total = f_motor - f_resRunning
```

**Where:**
- `f_motor` : Current motor force from field weakening curve (kN)
- `f_resRunning` : Total running resistance (kN)
- `f_total` : Net accelerating force (kN)

**During coasting:** `f_motor = 0`, so `f_total = -f_resRunning` (deceleration)

**Code Location:** `tractive_effort_handler.cpp` lines 16-20

---

### 3.4 Adhesion Limit
```cpp
tm_adh = (tm_f × cF) / ((mass_M2 + mass_P_M2) × 1000 / n_axle)
```

**Where:**
- `tm_f` : Force per traction motor (kN)
- `cF` : Conversion factor kN → kgf
- `mass_M2` : Mass on powered axles (tons)
- `n_axle` : Number of axles

**Code Location:** `tractive_effort_handler.cpp` lines 59-64

---

## 4. Resistance Formulas

**Source:** `backend/controllers_copy/simulation/resistance_handler.cpp`

### 4.1 Train Base Resistance
```cpp
r_train = (m × startRes) / 1000  // kN
```

**Where:**
- `m` : Train mass (tons)
- `startRes` : Specific starting resistance (N/ton)

**Code Location:** `resistance_handler.cpp` lines 8-11

---

### 4.2 Slope Resistance
```cpp
r_slope = (m_inertial × g × slope‰) / 1000  // kN
```

**Where:**
- `m_inertial` : Inertial mass (tons → kg)
- `g = 9.8` m/s²
- `slope‰` : Gradient in permille (‰)

**Example:** 10‰ = 1% grade = 0.01

**Code Location:** `resistance_handler.cpp` lines 13-16

---

### 4.3 Curve Resistance
```cpp
r_radius = (m_inertial × g × (6.0 / radius)) / 1000  // kN
```

**Where:**
- `radius` : Curve radius (m)
- `6.0` : Empirical constant for railway curves

**Code Location:** `resistance_handler.cpp` lines 18-21

---

### 4.4 Running Resistance (Davis Equation)
```cpp
r_run = (1/1000) × [
    (1.65 + 0.0247×v) × (mass_M × g) +              // Motor car resistance
    (0.78 + 0.0028×v) × (mass_T × g) +              // Trailer car resistance
    g × (0.028 + 0.0078×(n_car - 1)) × v²           // Air resistance
]
```

**Where:**
- `v` : Speed (km/h)
- `mass_M` : Motor car mass (tons)
- `mass_T` : Trailer car mass (tons)
- `n_car` : Number of cars
- Coefficients: Empirical values for rolling and air resistance

**This is the classical Davis equation adapted for electric trains.**

**Code Location:** `resistance_handler.cpp` lines 36-44

---

### 4.5 Total Starting Resistance
```cpp
f_resStart = r_train + r_slope + r_radius
```

**Code Location:** `resistance_handler.cpp` lines 23-31

---

### 4.6 Total Running Resistance
```cpp
f_resRunning = r_run + r_slope + r_radius
```

**Code Location:** `resistance_handler.cpp` lines 33-46

---

## 5. Power Calculations

**Source:** `backend/controllers_copy/simulation/power_handler.cpp`

### 5.1 Power at Wheel
```cpp
p_wheel = f_motor × (v / cV)  // kW
```

**Where:**
- `f_motor` : Motor force (kN)
- `v` : Speed (km/h)
- `cV = 3.6` : Conversion km/h → m/s

**Physics:** Power = Force × Velocity (in consistent units)

**Code Location:** `power_handler.cpp` lines 11-14

---

### 5.2 Efficiency Chain (Power Losses)

```cpp
p_motorOut = p_wheel / eff_gear          // Power output from gearbox
p_motorIn = p_motorOut / eff_motor       // Power input to motor
p_vvvfIn = p_motorIn / eff_vvvf          // Power input to VVVF inverter
p_catenary = p_vvvfIn + p_aps            // Total power from overhead line
```

**Where:**
- `eff_gear` : Gearbox efficiency (typically 0.95-0.98)
- `eff_motor` : Motor efficiency (typically 0.90-0.95)
- `eff_vvvf` : VVVF inverter efficiency (typically 0.95-0.98)
- `p_aps` : Auxiliary power (lights, HVAC, etc.) (kW)

**Code Locations:** `power_handler.cpp` lines 16-31

---

### 5.3 Power Flow Direction

**Powering (Acceleration/Constant Speed):**
```
Catenary → VVVF → Motor → Gearbox → Wheel → Train
```

**Regenerative Braking:**
```
Train → Wheel → Gearbox → Motor → VVVF → Catenary
(Power values become negative)
```

---

## 6. Mass Calculations

**Source:** `backend/controllers_copy/simulation/mass_handler.cpp`

### 6.1 Empty Train Mass
```cpp
mass_Me = (n_M1 × mass_M1) + (n_M2 × mass_M2)  // Motor cars
mass_Te = (n_Tc × mass_TC) + (n_T1 × mass_T1) + (n_T2 × mass_T2) + ...  // Trailer cars
mass_totalEmpty = mass_Me + mass_Te
```

**Where:**
- `n_M1`, `n_M2` : Number of motor car types
- `mass_M1`, `mass_M2` : Mass per motor car type (tons)
- `n_Tc`, `n_T1`, etc. : Number of trailer car types
- `mass_TC`, `mass_T1`, etc. : Mass per trailer car type (tons)

**Code Location:** `mass_handler.cpp` lines 8-16

---

### 6.2 Loaded Train Mass
```cpp
mass_P_final = mass_P / 1000  // Passenger mass (kg → tons)

mass_M = mass_Me + Σ(n_cars × mass_P_final × passengers_per_car)  // Motor cars
mass_T = mass_Te + Σ(n_cars × mass_P_final × passengers_per_car)  // Trailer cars

mass_totalLoad = mass_M + mass_T
```

**Alternative (if load per car is specified):**
```cpp
mass_totalLoad = mass_totalEmpty + (n_car × load_per_car)
```

**Code Locations:** `mass_handler.cpp` lines 18-31, 33-37

---

### 6.3 Inertial Mass (Rotational Effects)
```cpp
mass_Mi = (mass_Me × i_M) + (passenger_mass_in_motor_cars)
mass_Ti = (mass_Te × i_T) + (passenger_mass_in_trailer_cars)
mass_totalInertial = mass_Mi + mass_Ti
```

**Where:**
- `i_M` : Inertial coefficient for motor cars (typically 1.06-1.08)
- `i_T` : Inertial coefficient for trailer cars (typically 1.03-1.06)

**Physics:** Accounts for rotational inertia of wheels, axles, motors, and gears.

**Simplified (if using load input):**
```cpp
mass_totalInertial = (mass_Me × i_M) + (mass_Te × i_T) + (n_car × load)
```

**Code Locations:** `mass_handler.cpp` lines 41-58

---

## 7. Traction Motor Calculations

**Source:** `backend/controllers_copy/simulation/traction_motor_handler.cpp`

### 7.1 Force per Motor
```cpp
tm_f = f_motor / n_tm
```

**Where:**
- `f_motor` : Total motor force (kN)
- `n_tm` : Number of traction motors
- `tm_f` : Force per motor (kN)

**Code Location:** `traction_motor_handler.cpp` lines 11-14

---

### 7.2 Motor RPM
```cpp
tm_rpm = (v × 1000 / 60 × gearRatio) / (π × wheel / 1000)
```

**Simplified:**
```cpp
tm_rpm = (v × gearRatio × 1000) / (60 × π × wheel)
```

**Where:**
- `v` : Train speed (km/h)
- `gearRatio` : Gear reduction ratio (e.g., 6.06)
- `wheel` : Wheel diameter (mm)
- `π ≈ 3.14159`

**Code Location:** `traction_motor_handler.cpp` lines 16-18

---

### 7.3 Motor Torque
```cpp
tm_t = (f_motor × (wheel / 2000)) / gearRatio  // kNm
```

**Where:**
- `f_motor` : Total motor force (kN)
- `wheel` : Wheel diameter (mm)
- `gearRatio` : Gear reduction ratio
- Division by 2000 converts: mm → m and accounts for radius

**Code Location:** `traction_motor_handler.cpp` lines 20-23

---

### 7.4 Resistance Force per Motor
```cpp
tm_f_res = f_res / n_tm
```

**Code Location:** `traction_motor_handler.cpp` lines 25-28

---

## 8. Energy Calculations

**Source:** `backend/controllers_copy/simulation/energy_handler.cpp`

### 8.1 Energy Consumption per Time Step
```cpp
e_motor = (p_motorOut / 3600) × Δt  // kWh
```

**Where:**
- `p_motorOut` : Motor output power (kW)
- `Δt` : Time step (seconds, typically 0.1s)
- Division by 3600: seconds → hours

**Code Location:** `energy_handler.cpp` lines 9-13

---

### 8.2 Energy from Catenary (Powering)
```cpp
e_pow = (p_catenary / 3600) × Δt  // kWh
```

**Code Location:** `energy_handler.cpp` lines 15-19

---

### 8.3 Energy Regeneration (Braking)
```cpp
e_reg = (p_catenary / 3600) × Δt  // kWh (when braking)
```

**Note:** Power is negative during regeneration.

**Code Location:** `energy_handler.cpp` lines 21-25

---

### 8.4 Auxiliary Energy
```cpp
e_aps = (p_aps / 3600) × Δt  // kWh
```

**Code Location:** `energy_handler.cpp` lines 27-31

---

### 8.5 Total Energy (Cumulative)
```cpp
// During each time step:
e_motor_total += e_motor
e_pow_total += e_pow
e_reg_total += e_reg  // (negative)
e_aps_total += e_aps
e_catenary_total = e_pow_total + e_reg_total
```

---

## 9. Current Calculations

**Source:** `backend/controllers_copy/simulation/current_handler.cpp`

### 9.1 VVVF Input Current
```cpp
curr_vvvf = 1000 × (p_vvvfIn / vol_line)  // Amperes
```

**Where:**
- `p_vvvfIn` : VVVF input power (kW)
- `vol_line` : Line voltage (V)
- Factor 1000: kW → W

**Physics:** I = P / V (assuming unity power factor)

**Code Location:** `current_handler.cpp` lines 10-18

---

### 9.2 Catenary Current
```cpp
curr_catenary = 1000 × (p_catenary / vol_line)  // Amperes
```

**Code Location:** `current_handler.cpp` lines 20-28

---

### 9.3 Safety Checks
Both current calculations include checks for invalid voltage:
```cpp
if (vol_line <= 0.0 || isnan(vol_line) || isinf(vol_line)) {
    curr = 0.0;  // Avoid division by zero
}
```

---

## 10. Simulation Flow

### 10.1 Dynamic Simulation Main Loop

**Source:** `backend/controllers_copy/simulation/train_simulation_handler.cpp` lines 85-289

```
1. Initialize:
   - Calculate mass_totalEmpty, mass_totalLoad, mass_totalInertial
   - Set initial conditions: v = 0, acc = acc_start, position = 0
   
2. For each time step (Δt = 0.1s):
   
   a. Determine train state (notch):
      - AtStation: Dwelling at station
      - Accelerating: Applying motor force
      - Coasting: No motor force, coasting
      - Braking: Applying brakes
   
   b. Calculate track conditions:
      - Current slope, radius, speed limit
      - Update indices for CSV data
   
   c. Calculate resistances:
      - f_resStart (if v = 0)
      - f_resRunning (if v > 0)
   
   d. Calculate motor force:
      - If Accelerating: calculatePoweringForce(acc, v)
        → Uses field weakening curve (v_p1, v_p2)
      - If Braking: calculateBrakingForce()
        → Uses braking curve (v_b1, v_b2)
      - If Coasting: f_motor = 0
   
   e. Calculate net force:
      - f_total = f_motor - f_resRunning
   
   f. Calculate acceleration:
      - acc_si = f_total / mass_totalInertial  (m/s²)
      - acc = acc_si × cV  (km/h/s)
   
   g. Update velocity:
      - v_si += acc_si × Δt  (m/s)
      - v += acc × Δt  (km/h)
   
   h. Update position:
      - x = v_prev × Δt + 0.5 × acc_prev × Δt²
      - x_total += x
   
   i. Calculate powers:
      - p_wheel = f_motor × (v / cV)
      - Apply efficiency chain → p_catenary
   
   j. Calculate energies:
      - e_motor, e_pow, e_reg, e_aps
   
   k. Calculate motor parameters:
      - tm_f, tm_rpm, tm_t, tm_adh
   
   l. Calculate electrical parameters:
      - curr_vvvf, curr_catenary
   
   m. Store data:
      - Append to simulation vectors
   
   n. Check station arrival:
      - If x_total >= x_station[next]: notch = Braking
      - If v = 0 at station: notch = AtStation
   
3. End loop when all stations visited

4. Emit simulationCompleted signal
```

---

### 10.2 Static Simulation (Speed vs Performance)

**Source:** `train_simulation_handler.cpp` lines 291-371

```
1. Initialize same as dynamic

2. For each speed increment (v++):
   
   a. Calculate resistances at current speed
   
   b. Calculate motor force at current speed
   
   c. Calculate acceleration capability
   
   d. Calculate powers, energies, currents
   
   e. Store data
   
3. End when v >= v_limit or x >= x_station
```

**Purpose:** Generate speed vs power/current curves for motor specification.

---

## 11. Motor Power vs Simulation Variables

### 11.1 Thesis Formula (Specification Calculator)

**Your Formula:**
```
P_motor_kW = (m × a × v_weak) / (n × 1000)
```

**Where:**
- `m` : Train mass (kg)
- `a` : Target acceleration (m/s²)
- `v_weak` : Weakening point speed (m/s)
- `n` : Number of motors
- `P_motor_kW` : Required motor power rating **per motor**

**Purpose:** Calculate what motor power rating to buy/specify.

**Example:**
```
m = 280,000 kg (280 tons)
a = 1.0 m/s²
v_weak = 30 m/s (108 km/h, at v_p1)
n = 4 motors

P_motor = (280,000 × 1.0 × 30) / (4 × 1000)
        = 8,400,000 / 4,000
        = 2,100 kW per motor
```

---

### 11.2 Simulation's Power Calculation (Derived Output)

**Simulation Formula:**
```cpp
p_wheel = f_motor × (v / cV)  // Total power at wheels

// Where f_motor comes from:
f_start = mass_totalInertial × acc_start + f_resStart

// At constant power region (v > v_p1):
f_motor = (f_start × v_p1) / v
```

**Substituting:**
```
p_wheel = [(mass × acc_start × v_p1) / v] × (v / cV)
        = (mass × acc_start × v_p1) / cV
        ≈ m × a × v_weak  (when cV conversion applied correctly)
```

**Key Insight:** The simulation derives power from force and velocity, while your formula specifies the required power directly.

---

### 11.3 Relationship Between Formulas

**Converting Thesis Formula → Simulation Input:**

```cpp
// Given: Desired P_motor_kW per motor
// Need: acc_start for simulation

// From thesis formula:
P_total_kW = P_motor_kW × n_tm

// At weakening point (v = v_p1), motor operates at constant power:
// P = f_start × v_p1
// f_start = mass_totalInertial × acc_start (ignoring resistance)
// Therefore: P = mass × acc_start × v_p1

// Solving for acc_start:
acc_start = P_total_kW / (mass_totalInertial × v_p1)

// In code:
double v_p1_ms = movingData->v_p1 / 3.6;  // km/h → m/s
double mass_kg = massData->mass_totalInertial × 1000;  // tons → kg
movingData->acc_start = (P_total_kW × 1000) / (mass_kg × v_p1_ms);  // m/s²
```

**Converting Simulation Output → Motor Power:**

```cpp
// From simulation results at v = v_p1:
double p_total_kW = powerData->p_wheel;  // At weakening point
double P_per_motor_kW = p_total_kW / trainData->n_tm;
```

---

## 12. Optimization Strategy

### 12.1 Three-Stage Pipeline

#### Stage 1: Motor Sizing Calculator (Baseline Calculation)

**Input from User:**
- `mass_totalInertial` (tons or kg)
- `targetAccel` (m/s²) - desired acceleration capability
- `v_weak` (m/s or km/h) - weakening point speed
- `n_tm` - number of traction motors

**Calculation:**
```cpp
double P_baseline_per_motor = (mass_kg × targetAccel × v_weak_ms) / (n_tm × 1000);
double P_baseline_total = P_baseline_per_motor × n_tm;
```

**Output:**
- `P_baseline_kW` - baseline motor power specification
- Sweep range: `[P_baseline × 0.8, P_baseline × 1.2]` (±20%)

---

#### Stage 2: Physics Simulation (Accepts acc_start)

**Convert Power → Acceleration:**
```cpp
double acc_start_required = (P_total_kW × 1000) / (mass_kg × v_weak_ms);
```

**Set in simulation:**
```cpp
movingData->acc_start = acc_start_required;  // m/s²
```

**Run simulation:**
```cpp
runDynamicSimulation();  // Uses real physics engine
```

**Extract outputs:**
```cpp
double totalTime = getMaxCurrTime();
double totalEnergy = energyData->e_catenary;
double avgAccel = /* calculate from simulation data */;
double actualWeakeningPoint = movingData->v_p1;
```

---

#### Stage 3: Parameter Sweep with Fuzzy Evaluation

```cpp
// Sweep motor power from -20% to +20%
double P_min = P_baseline × 0.8;
double P_max = P_baseline × 1.2;
double P_step = 5.0;  // kW

double bestScore = -1.0;
double optimalPower = P_baseline;

for (double P_test = P_min; P_test <= P_max; P_test += P_step) {
    
    // 1. Convert to acceleration
    double acc_test = (P_test × 1000) / (mass_kg × v_weak_ms);
    
    // 2. Set in simulation
    movingData->acc_start = acc_test;
    
    // 3. Run simulation
    runDynamicSimulation();
    
    // 4. Extract metrics
    double travelTime = getTotalTime();
    double energyUsed = getTotalEnergy();
    double avgAccel = /* from data */;
    double maxGradient = /* from track */;
    double speedLimit = /* from track */;
    
    // 5. Evaluate with Fuzzy Logic (Mamdani)
    fuzzyEngine->setInputValue("Acceleration", avgAccel);
    fuzzyEngine->setInputValue("WeakeningPoint", v_p1);
    fuzzyEngine->setInputValue("Gradient", maxGradient);
    fuzzyEngine->setInputValue("SpeedLimit", speedLimit);
    
    double score = fuzzyEngine->getOutputValue("Suitability");
    
    // 6. Track best (DO NOT store full graph data yet!)
    if (score > bestScore) {
        bestScore = score;
        optimalPower = P_test;
        optimalAccel = acc_test;
    }
}

// 7. ONLY NOW run final simulation with optimal values for full data
movingData->acc_start = optimalAccel;
runDynamicSimulation();
// Store full graph data, generate CSV, etc.
```

---

### 12.2 RAM Optimization

**During Sweep (Lightweight Storage):**
```cpp
struct IterationResult {
    double motorPower_kW;
    double fuzzyScore;
    double travelTime_min;
    double energyUsed_kWh;
    // DO NOT store vectors!
};

QVector<IterationResult> sweepHistory;
```

**Final Result (Full Data):**
```cpp
struct OptimizationResult {
    double optimalPower_kW;
    double optimalAccel_ms2;
    double suitabilityScore;
    QString suitabilityLabel;
    
    // Full simulation data (only for winner)
    QVector<double> speeds;
    QVector<double> distances;
    QVector<double> times;
    QVector<double> powers;
    QVector<double> currents;
    
    // Sweep history (lightweight)
    QVector<IterationResult> sweepHistory;
};
```

**Memory Savings:**
- Before: ~16 MB (40 iterations × full data)
- After: ~401 KB (40 iterations × 4 doubles + 1 full result)

---

### 12.3 Fuzzy Logic Evaluation (Mamdani)

**Already Implemented:** `backend/utils/fuzzy_engine.cpp`

**Inputs:**
1. `Acceleration` (m/s²) - Average or starting acceleration
2. `WeakeningPoint` (km/h) - v_p1 value
3. `Gradient` (‰) - Maximum track gradient
4. `SpeedLimit` (km/h) - Maximum allowed speed

**Output:**
- `Suitability` (0.0 - 1.0) - Motor specification quality score

**Method:**
- Fuzzification: Triangle/Trapezoid membership functions
- Inference: MIN (AND), MAX (OR/aggregation)
- Defuzzification: Centroid method

**Membership Functions (Example from code):**
```cpp
// Acceleration: Low [0, 0, 0.8], Medium [0.5, 1.0, 1.5], High [1.2, 3.0, 3.0]
// WeakeningPoint: Low [0, 0, 35], Medium [30, 45, 60], High [50, 120, 120]
// Gradient: Low [0, 0, 5], Medium [2, 10, 20], High [15, 40, 40]
// SpeedLimit: Low [0, 0, 60], Medium [50, 80, 110], High [100, 160, 160]
```

**Rules (Example):**
```
IF Acceleration = Low AND Gradient = High THEN Suitability = NotSuitable
IF Acceleration = Medium AND WeakeningPoint = Medium THEN Suitability = QuiteSuitable
IF Acceleration = High AND WeakeningPoint = High THEN Suitability = VerySuitable
```

---

## Summary

### Key Takeaways

1. **Motor power is DERIVED, not INPUT** in the simulation
   - Simulation accepts: `acc_start` (m/s²)
   - Simulation outputs: `p_wheel`, `p_motor`, etc. (kW)

2. **Unit conversions are critical**
   - `acc_start` is in m/s²
   - `acc` is in km/h/s
   - `acc_si` is in m/s² (for calculations)

3. **Field weakening curve is the CORE of motor behavior**
   - 3 phases: Constant torque, constant power, constant power²
   - Controlled by `v_p1` and `v_p2`

4. **Optimization should sweep `acc_start`, not power directly**
   - Calculate baseline power using thesis formula
   - Convert to `acc_start` for simulation
   - Sweep ±20% around baseline
   - Use fuzzy logic to evaluate each iteration

5. **Mamdani fuzzy logic is already correctly implemented**
   - Keep it (perfect for thesis defense)
   - Only needs to be connected to optimization loop

---

## Next Steps

1. ✅ Documentation complete
2. ⏭️ Implement Stage 1: Motor Sizing Calculator
3. ⏭️ Implement Stage 2: Modify optimization handler to use real simulation
4. ⏭️ Implement Stage 3: Parameter sweep loop with RAM optimization
5. ⏭️ Create API endpoints for frontend integration
6. ⏭️ Build frontend UI for optimization workflow

---

**Document Version:** 1.0  
**Last Updated:** February 3, 2026  
**Author:** Analysis of existing codebase for fuzzy optimization implementation
