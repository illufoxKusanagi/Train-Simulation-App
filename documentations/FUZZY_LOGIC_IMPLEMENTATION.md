# Fuzzy Logic Implementation & Thesis Mapping

This document details how the C++ implementation realizes the design specified in the thesis **"SIMULASI OPTIMASI SPESIFIKASI KERETA API LISTRIK MENGGUNAKAN LOGIKA FUZZY"**.

## 1. System Architecture (Thesis Chapter 3)

The implementation follows the **"Mechanic & Judge"** pattern to realize the optimization loop described in **Section 3.2.3 (Optimasi dengan Logika Fuzzy)**.

- **The Judge**: `FuzzyEngine` (Corresponds to **Section 3.2.3.3 Inference Engine**)
- **The Mechanic**: `OptimizationHandler` (Corresponds to the iterative simulation process implied in **Section 3.2.3**)

---

## 2. Fuzzy Logic Implementation (Thesis Section 3.2.3)

The core logic is implemented in `backend/utils/fuzzy_engine.cpp`.

### 2.1. Fuzzification (Section 3.2.3.1)

**Thesis Concept**: Transforming crisp numerical values (Acceleration, Weakening Point, etc.) into fuzzy membership degrees using Triangle or Trapezoid functions.

**Implementation**:

- **Class**: `FuzzySet` (Base), `TriangleSet`, `TrapezoidSet`.
- **Code Mapping**:
  ```cpp
  // Implements Equation 3.1: Triangle Membership Function
  double TriangleSet::membership(double value) const {
      if (value <= a || value >= c) return 0.0;
      if (value == b) return 1.0;
      if (value < b) return (value - a) / (b - a); // Rising edge
      return (c - value) / (c - b);               // Falling edge
  }
  ```
- **Variables Configured** (in `OptimizationHandler::setupFuzzyEngine`):
  - `Acceleration`: Low, Medium, High (Matches **Figure 3.3**)
  - `WeakeningPoint`: Low, Medium, High (Matches **Figure 3.4**)
  - `Gradient`: Low, Medium, High (Matches **Figure 3.5**)
  - `SpeedLimit`: Low, Medium, High (Matches **Figure 3.6**)

### 2.2. Knowledge Base (Section 3.2.3.2)

**Thesis Concept**: A set of IF-THEN rules derived from domain knowledge.

**Implementation**:

- **Class**: `FuzzyRule`.
- **Code Mapping**:
  Rules are added in `OptimizationHandler::setupFuzzyEngine`.
  ```cpp
  // Example from Thesis Section 3.2.3.5 (Rule b)
  // IF Acceleration IS Medium AND Weakening IS Medium AND Slope IS Medium THEN Quite Suitable
  m_fuzzyEngine->addRule({{{"Acceleration", "Medium"},
                           {"WeakeningPoint", "Medium"},
                           {"Gradient", "Medium"}},
                          {"Suitability", "QuiteSuitable"}});
  ```

### 2.3. Inference Engine (Section 3.2.3.3)

**Thesis Concept**: Evaluating fired rules using the **Mamdani** method.

- **AND Operator**: Minimum (`std::min`).
- **Aggregation**: Maximum (`std::max`) of rule outputs.

**Implementation**:

- **Method**: `FuzzyEngine::evaluate()`.
- **Code Mapping**:

  ```cpp
  // 1. Calculate Firing Strength (AND / MIN)
  double firingStrength = 1.0;
  for (auto const &[varName, termName] : rule.inputs) {
      double membership = inputVar->getSet(termName)->membership(inputValue);
      firingStrength = std::min(firingStrength, membership);
  }

  // 2. Aggregate (OR / MAX) - Implicitly handled during Defuzzification loop
  // by taking the max contribution at each x step.
  ```

### 2.4. Defuzzification (Section 3.2.3.4)

**Thesis Concept**: Converting the aggregated fuzzy set back to a crisp value using the **Centroid** method (Equation 2.12).

**Implementation**:

- **Method**: `FuzzyEngine::evaluate()` (End of function).
- **Code Mapping**:
  ```cpp
  // Implements Equation 2.12: z = (∫ μ(z) * z dz) / (∫ μ(z) dz)
  // Discretized as: Sum(x * mu) / Sum(mu)
  for (double x = minVal; x <= maxVal; x += step) {
      // ... calculate aggregated maxMembership at x ...
      numerator += x * maxMembership;
      denominator += maxMembership;
  }
  return (denominator == 0) ? 0 : numerator / denominator;
  ```

---

## 3. Optimization Workflow (Thesis Section 3.2.2 & 3.2.3)

The optimization process finds the most cost-effective motor power specification by systematically varying train design parameters and evaluating each configuration using the Fuzzy Engine.

### 3.1. Design Parameters (Variables to Optimize)

The system optimizes **5 design parameters** to find the optimal motor power per unit:

1. **Acceleration** (m/s²) - Starting acceleration capability
2. **Weakening Point** (km/h) - Field weakening transition speed
3. **Top Speed** (km/h) - Maximum design speed of the train
4. **Train Weight** (tons) - Total mass (cars + passengers)
5. **Number of Motors** - Motor units in the train

**Fixed Constraints** (from track data):

- Track gradients, curves, distances (CSV files)
- Speed limits per section
- Route characteristics

### 3.2. Optimization Strategy: Coarse-to-Fine Parameter Sweep

**Goal**: Find the motor power specification with the best cost-performance balance.

**Method**: Multi-simulation parameter sweep with fuzzy evaluation.

#### Stage 1: Coarse Grid Search (~40 runs)

```
Parameter Ranges:
  - Acceleration: [0.8, 1.0, 1.2] m/s² (3 values)
  - Weakening Point: [70, 85, 100] km/h (3 values)
  - Number of Motors: [4, 6, 8] units (3 values)
  - Weight/Top Speed: Use baseline values from initial specs

Total: 3 × 3 × 3 = 27 combinations

For each combination:
  1. Run full physics simulation (TrainSimulationHandler::runDynamicSimulation)
  2. Extract motor power per unit from simulation results
  3. Calculate travel time, energy consumption
  4. Fuzzy Engine evaluates → Suitability score (0.0-1.0)
  5. Store summary: {params, motor_power, score, time, energy} (~2KB)

Result: Top 3 best configurations
```

#### Stage 2: Refinement (~90 runs)

```
For each of the top 3 from Stage 1:
  - Create ±10% range around the best values
  - Use finer steps
  - 30 combinations per region

Total: 3 regions × 30 = 90 runs

Result: Best configuration with higher precision
```

#### Stage 3: Final Tuning (~30 runs)

```
Around the single best configuration:
  - Very fine adjustments (±5% range)
  - Finest step size

Total: ~30 runs

Result: OPTIMAL train specification
```

### 3.3. RAM Optimization (Critical for Low-End Hardware)

**Challenge**: Full simulation data per run = ~100KB × 150 runs = 15MB
**Target Hardware**: AMD A6 7310 with 4GB RAM

**Solution**: Store only summary data during sweep

```cpp
struct OptimizationResult {
    // Design Parameters (20 bytes)
    double acceleration;
    double weakeningPoint;
    double topSpeed;
    double trainWeight;
    int numMotors;

    // Performance Metrics (32 bytes)
    double motorPowerPerUnit;  // PRIMARY OUTPUT SPEC
    double fuzzyScore;
    double travelTime;
    double energyConsumption;
};
// Total: ~52 bytes per configuration
// 150 runs × 52 bytes = ~8KB (vs 15MB!)
```

**Full Data Retrieval**:

- After sweep completes, re-run the WINNER configuration only
- Store complete simulation data for detailed analysis/graphs

### 3.4. Parallel Execution Strategy

**Hardware**: 4-core CPU (AMD A6 7310)
**Strategy**: Run 2 simulations in parallel (reserve 2 cores for OS)

**Performance Estimate**:

- Single simulation: ~1.5 seconds
- Total runs: 150 (coarse-to-fine)
- Parallel factor: 2
- **Total time: ~2 minutes**

**Memory Safety**:

- Clear simulation data after each run
- Mutex-protected shared data access
- Maximum concurrent memory: ~200KB

### 3.5. Workflow Implementation

```cpp
// Pseudo-code for optimization loop
std::vector<OptimizationResult> results;

// Stage 1: Coarse Search
for (double accel : {0.8, 1.0, 1.2}) {
    for (double weakening : {70, 85, 100}) {
        for (int motors : {4, 6, 8}) {
            // Set train parameters
            trainData->acc_start_si = accel;
            trainData->v_weakening = weakening;
            trainData->n_tm = motors;

            // Run real simulation (NOT approximations)
            TrainSimulationHandler::runDynamicSimulation();

            // Extract motor power from simulation results
            double motorPower = extractMotorPowerFromSimulation();

            // Fuzzy evaluation
            double score = fuzzyEngine->evaluate({
                {"Acceleration", accel},
                {"WeakeningPoint", weakening},
                {"Gradient", trackData->maxGradient},
                {"SpeedLimit", trackData->maxSpeedLimit}
            });

            // Store summary only
            results.push_back({accel, weakening, topSpeed,
                              weight, motors, motorPower,
                              score, time, energy});

            // Clear simulation data to free RAM
            clearSimulationData();
        }
    }
}

// Find top 3
auto top3 = getTopN(results, 3);

// Stage 2 & 3: Refine around best configurations
// ... similar process with finer ranges ...

// Final: Re-run winner for full data
auto winner = getBest(results);
applyConfiguration(winner);
runDynamicSimulation(); // Keep full data this time
```

---

## 4. Key Implementation Principles

### 4.1. Real Physics Simulation (NOT Approximations)

- Uses `TrainSimulationHandler::runDynamicSimulation()`
- Full field weakening motor model (3 phases)
- Actual resistance calculations (Davis equation)
- Real braking physics with deficit compensation
- Track profile integration (gradients, curves, speed limits)

### 4.2. Fuzzy as Evaluator (NOT Controller)

- Fuzzy Engine **scores** configurations after simulation
- Does NOT modify parameters during simulation
- Evaluation happens AFTER physics completes
- Score guides the sweep algorithm to explore better regions

### 4.3. Motor Power Extraction

Motor power is **derived from simulation**, not calculated separately:

```cpp
// During simulation, at field weakening speed:
double motorPowerPerUnit = powerData->p_wheel / trainMotorData->n_tm;
```

Where `p_wheel` comes from:

```cpp
p_wheel = f_motor × (v / 3.6)  // From power_handler.cpp
```

And `f_motor` comes from physics:

```cpp
f_start = mass × acc_start_si + f_resStart
// → field weakening curve → f_motor
```

### 4.4. Cost-Performance Balance

The fuzzy system naturally balances cost vs performance:

- **High acceleration** → Higher motor power → Higher cost → Lower fuzzy score (if excessive)
- **Low acceleration** → Lower motor power → Lower cost → But longer travel time → Lower score
- **Optimal zone** → Minimum motor power that meets reasonable travel time → **Highest fuzzy score**

## 5. Case Study Verification (Thesis Section 3.2.3.5)

The Fuzzy Engine has been verified against the example case in **Section 3.2.3.5**:

- **Input**:
  - Acceleration: 0.6 m/s²
  - Weakening Point: 27 km/h (Thesis says 31 km/h in text, 27 in table. We handle both ranges).
  - Gradient: 4‰
  - Speed Limit: 90 km/h
- **Process**:
  - Fuzzification correctly identifies "Low" (0.8) and "Medium" (0.1) for Acceleration.
  - Rules fire as expected.
  - Defuzzification yields ~0.55.
- **Result**: The code produces the same "Quite Suitable" classification.

## 6. Expected Optimization Results

**Example Output**:

```
BASELINE (from initial specs):
  Acceleration: 1.0 m/s²
  Weakening Point: 85 km/h
  Train Weight: 300 tons
  Number of Motors: 6
  → Motor Power: 292 kW/unit
  → Fuzzy Score: 0.72
  → Travel Time: 18.5 min

OPTIMAL (after sweep):
  Acceleration: 0.95 m/s²
  Weakening Point: 82 km/h
  Train Weight: 285 tons
  Number of Motors: 6
  → Motor Power: 270 kW/unit (7.5% CHEAPER!)
  → Fuzzy Score: 0.88 (BEST)
  → Travel Time: 19.2 min (only 42s slower)

Cost Savings: ~22 kW/unit × 6 motors = 132 kW total reduction
```

**Thesis Contribution**: Demonstrates that fuzzy logic optimization can find cost-effective train specifications that traditional heuristic methods might miss.
