# Fuzzy Logic Implementation & Thesis Mapping

This document details how the C++ implementation realizes the design specified in the thesis **"SIMULASI OPTIMASI SPESIFIKASI KERETA API LISTRIK MENGGUNAKAN LOGIKA FUZZY"**.

## 1. System Architecture (Thesis Chapter 3)

The implementation follows the **"Mechanic & Judge"** pattern to realize the optimization loop described in **Section 3.2.3 (Optimasi dengan Logika Fuzzy)**.

*   **The Judge**: `FuzzyEngine` (Corresponds to **Section 3.2.3.3 Inference Engine**)
*   **The Mechanic**: `OptimizationHandler` (Corresponds to the iterative simulation process implied in **Section 3.2.3**)

---

## 2. Fuzzy Logic Implementation (Thesis Section 3.2.3)

The core logic is implemented in `backend/utils/fuzzy_engine.cpp`.

### 2.1. Fuzzification (Section 3.2.3.1)

**Thesis Concept**: Transforming crisp numerical values (Acceleration, Weakening Point, etc.) into fuzzy membership degrees using Triangle or Trapezoid functions.

**Implementation**:
*   **Class**: `FuzzySet` (Base), `TriangleSet`, `TrapezoidSet`.
*   **Code Mapping**:
    ```cpp
    // Implements Equation 3.1: Triangle Membership Function
    double TriangleSet::membership(double value) const {
        if (value <= a || value >= c) return 0.0;
        if (value == b) return 1.0;
        if (value < b) return (value - a) / (b - a); // Rising edge
        return (c - value) / (c - b);               // Falling edge
    }
    ```
*   **Variables Configured** (in `OptimizationHandler::setupFuzzyEngine`):
    *   `Acceleration`: Low, Medium, High (Matches **Figure 3.3**)
    *   `WeakeningPoint`: Low, Medium, High (Matches **Figure 3.4**)
    *   `Gradient`: Low, Medium, High (Matches **Figure 3.5**)
    *   `SpeedLimit`: Low, Medium, High (Matches **Figure 3.6**)

### 2.2. Knowledge Base (Section 3.2.3.2)

**Thesis Concept**: A set of IF-THEN rules derived from domain knowledge.

**Implementation**:
*   **Class**: `FuzzyRule`.
*   **Code Mapping**:
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
*   **AND Operator**: Minimum (`std::min`).
*   **Aggregation**: Maximum (`std::max`) of rule outputs.

**Implementation**:
*   **Method**: `FuzzyEngine::evaluate()`.
*   **Code Mapping**:
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
*   **Method**: `FuzzyEngine::evaluate()` (End of function).
*   **Code Mapping**:
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

The `OptimizationHandler` orchestrates the process described in **Section 3.2.2 (Simulasi)** and uses the Fuzzy Engine to guide decisions.

### 3.1. Headless Simulation (Section 3.2.2)
**Thesis Concept**: Simulating train physics to get performance metrics.
**Implementation**: `OptimizationHandler::runHeadlessSimulation`.
*   Calculates `Acceleration` using `F = ma` (Equation 2.1).
*   Calculates `Weakening Point` based on motor characteristics.

### 3.2. The Optimization Loop
**Thesis Concept**: Finding the "Optimal Specification".
**Implementation**: `OptimizationHandler::runOptimizationLoop`.
1.  **Simulate**: Get metrics for current `TrainData`.
2.  **Evaluate**: Get `Suitability` score from `FuzzyEngine`.
3.  **Check**: If `Score >= 0.85` (Very Suitable), Stop.
4.  **Adjust**: If not, modify `Gear Ratio` or `Number of Motors` based on *which* input caused the low score (Heuristic approach).

---

## 4. Case Study Verification (Thesis Section 3.2.3.5)

The implementation has been verified against the example case in **Section 3.2.3.5**:

*   **Input**:
    *   Acceleration: 0.6 m/s²
    *   Weakening Point: 27 km/h (Thesis says 31 km/h in text, 27 in table. We handle both ranges).
    *   Gradient: 4‰
    *   Speed Limit: 90 km/h
*   **Process**:
    *   Fuzzification correctly identifies "Low" (0.8) and "Medium" (0.1) for Acceleration.
    *   Rules fire as expected.
    *   Defuzzification yields ~0.55.
*   **Result**: The code produces the same "Quite Suitable" classification.
