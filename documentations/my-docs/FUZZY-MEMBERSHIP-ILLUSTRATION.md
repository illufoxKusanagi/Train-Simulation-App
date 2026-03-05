# Fuzzy Logic Membership Functions Illustration

**For Train Specification Optimization System**  
**Date**: February 24, 2026

---

## Overview

The fuzzy system evaluates train design specifications to determine their suitability for a given route. It uses **4 input variables** and produces **1 output score**.

---

## 1. Input Variables (Design Parameters)

These are the parameters we can VARY during optimization:

### A. Acceleration (m/s²)

```
Membership
    1.0 |        Medium
        |       /‾‾‾‾\
    0.8 |  Low /      \ High
        |     /        \     
    0.6 |    /          \    
        |   /            \   
    0.4 |  /              \  
        | /                \ 
    0.2 |/                  \
    0.0 +--------------------+-----> Acceleration (m/s²)
        0.4  0.6  0.8  1.0  1.2  1.4

Low:    Triangle  [0.4, 0.6, 0.8]    ← Weak acceleration
Medium: Triangle  [0.6, 1.0, 1.2]    ← Balanced
High:   Triangle  [1.0, 1.2, 1.4]    ← Strong acceleration
```

**Optimization Range**: 0.8 - 1.2 m/s² (based on baseline ±20%)

**Physical Meaning**:
- Low (0.6): Gentle, comfortable, low motor power, SLOW
- Medium (1.0): Balanced performance and comfort
- High (1.2): Aggressive, high motor power, FAST

---

### B. Weakening Point (km/h)

```
Membership
    1.0 |        Medium
        |       /‾‾‾‾‾‾\
    0.8 |  Low /        \ High
        |     /          \     
    0.6 |    /            \    
        |   /              \   
    0.4 |  /                \  
        | /                  \ 
    0.2 |/                    \
    0.0 +----------------------+-----> Weakening Point (km/h)   
        50   70   85  100  120  140

Low:    Triangle  [50, 70, 85]      ← Early field weakening
Medium: Triangle  [70, 85, 100]     ← Balanced
High:   Trapezoid [85, 100, 120, 140] ← Late field weakening
```

**Optimization Range**: 70 - 100 km/h

**Physical Meaning**:
- Low (70): Constant torque ends early, wider constant power range
- Medium (85): Balanced motor characteristics
- High (100): Constant torque longer, higher top speed potential

---

### C. Gradient (‰) - Track Constraint

```
Membership
    1.0 |        Medium
        |       /‾‾‾‾\
    0.8 |  Low /      \ High
        |     /        \     
    0.6 |    /          \    
        |   /            \   
    0.4 |  /              \  
        | /                \ 
    0.2 |/                  \
    0.0 +--------------------+-----> Gradient (‰)
        0    5    10   20   30   40

Low:    Triangle  [0, 5, 10]       ← Flat terrain
Medium: Triangle  [5, 10, 20]      ← Moderate hills
High:   Triangle  [10, 30, 40]     ← Steep gradients
```

**Fixed by Route**: Cannot be changed, read from CSV track data

**Physical Meaning**:
- Low (5‰): Flat route, easier for train
- Medium (10‰): Normal hills
- High (30‰): Steep climb, requires more power

---

### D. Speed Limit (km/h) - Track Constraint

```
Membership
    1.0 |        Medium
        |       /‾‾‾‾‾‾\
    0.8 |  Low /        \ High
        |     /          \     
    0.6 |    /            \    
        |   /              \   
    0.4 |  /                \  
        | /                  \ 
    0.2 |/                    \
    0.0 +----------------------+-----> Speed Limit (km/h)
        40   60   80  100  120  140

Low:    Triangle  [40, 60, 80]     ← Urban/slow zones
Medium: Triangle  [60, 90, 110]    ← Suburban
High:   Trapezoid [90, 110, 130, 140] ← High-speed line
```

**Fixed by Route**: Cannot be changed, read from CSV track data

**Physical Meaning**:
- Low (60): Urban areas, frequent stops
- Medium (90): Typical commuter line
- High (120): Express/intercity line

---

## 2. Output Variable (Evaluation Result)

### Suitability Score (0.0 - 1.0)

```
Membership
    1.0 |                    Very Suitable
        |                   /‾‾‾‾‾‾‾‾‾‾‾‾\
    0.8 |        Suitable  /              \
        |       /‾‾‾‾‾‾‾\ /                \
    0.6 |      /         X                  \
        |     /         / \                  \
    0.4 |    /   Quite /   \                 \
        |   /  Suitable     \                 \
    0.2 |  /   /             \                 \
        | /   /               \                 \
    0.0 +----+----------------+----------------+---> Score
        0.0  0.3   0.5   0.7  0.85  1.0

Not Suitable:   Triangle  [0.0, 0.0, 0.3]    ← Reject this config
Quite Suitable: Triangle  [0.0, 0.3, 0.5]    ← Poor but workable
Suitable:       Triangle  [0.3, 0.5, 0.7]    ← Acceptable
Very Suitable:  Trapezoid [0.5, 0.7, 0.85, 1.0] ← Optimal!
```

**Decision Threshold**: Score ≥ 0.70 = Good configuration

**Physical Meaning**:
- < 0.3: Configuration won't work (too slow, too much power, etc.)
- 0.3-0.5: Works but inefficient
- 0.5-0.7: Good balance
- > 0.7: Excellent cost-performance ratio ⭐

---

## 3. Example Fuzzification

### Scenario: Testing Acceleration = 0.9 m/s²

```
Input Value: 0.9 m/s²

Step 1: Calculate memberships
        
        Low Triangle [0.6, 0.8, 1.0]:
             0.9 is between 0.8 and 1.0
             μ_Low = (1.0 - 0.9) / (1.0 - 0.8) = 0.1 / 0.2 = 0.5
        
        Medium Triangle [0.8, 1.0, 1.2]:
             0.9 is between 0.8 and 1.0 (rising edge)
             μ_Medium = (0.9 - 0.8) / (1.0 - 0.8) = 0.1 / 0.2 = 0.5
        
        High Triangle [1.0, 1.2, 1.4]:
             0.9 is below 1.0 (minimum)
             μ_High = 0.0

Result: Acceleration 0.9 m/s² is:
        - 50% "Low"
        - 50% "Medium"  
        - 0% "High"
```

**Visual**:
```
    1.0 |        ●
        |       /|‾\
    0.5 |    ● / |  \ 
        |   /| /  |   \
    0.0 |  / |/   |    \
        +----+----+----+----> Accel
        0.6 0.8  1.0  1.2
            ↑
           0.9
```

---

## 4. Complete Fuzzy Rule Example

### Rule: "Medium acceleration on moderate gradient is Suitable"

```
IF Acceleration = Medium (0.5)
AND Weakening = Medium (0.8)
AND Gradient = Medium (0.6)
AND SpeedLimit = Medium (0.7)
THEN Suitability = Suitable

Step 1: AND operation (use MIN)
        Firing Strength = min(0.5, 0.8, 0.6, 0.7) = 0.5

Step 2: Output membership
        "Suitable" fuzzy set activated with strength 0.5
        
        1.0 |
            |    /\
        0.5 |   /‾‾\      ← Clipped at 0.5
            |  /    \
        0.0 +--------+---> Suitability
            0.5  0.7
```

---

## 5. Optimization Process Flow

```
┌─────────────────────────────────────────────────────┐
│  Step 1: Define Parameter Ranges                    │
│  - Acceleration: [0.8, 1.0, 1.2]                   │
│  - Weakening: [70, 85, 100]                        │
│  - Read Gradient & SpeedLimit from track CSV       │
└─────────────────┬───────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────┐
│  Step 2: For Each Combination                       │
│  ┌──────────────────────────────────────────────┐  │
│  │  Set: accel=1.0, weakening=85               │  │
│  │  Run: Physics simulation                      │  │
│  │  Get: Motor power, travel time, energy       │  │
│  └──────────────────────────────────────────────┘  │
└─────────────────┬───────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────┐
│  Step 3: Fuzzification                              │
│  ┌──────────────────────────────────────────────┐  │
│  │  Acceleration 1.0 → Medium(1.0), High(0.0)  │  │
│  │  Weakening 85 → Low(0.0), Medium(1.0)       │  │
│  │  Gradient 10 → Medium(1.0)                   │  │
│  │  SpeedLimit 90 → Medium(1.0)                 │  │
│  └──────────────────────────────────────────────┘  │
└─────────────────┬───────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────┐
│  Step 4: Rule Evaluation (Mamdani)                  │
│  ┌──────────────────────────────────────────────┐  │
│  │  IF Medium & Medium & Medium & Medium        │  │
│  │  THEN Suitable                                │  │
│  │  Firing: min(1.0,1.0,1.0,1.0) = 1.0         │  │
│  │  (20 more rules evaluated...)                 │  │
│  └──────────────────────────────────────────────┘  │
└─────────────────┬───────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────┐
│  Step 5: Defuzzification (Centroid)                 │
│  ┌──────────────────────────────────────────────┐  │
│  │  Aggregate all fired rules (MAX)             │  │
│  │  Calculate center of mass                     │  │
│  │  Result: Suitability = 0.78                   │  │
│  └──────────────────────────────────────────────┘  │
└─────────────────┬───────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────┐
│  Step 6: Store Result                               │
│  {                                                   │
│    acceleration: 1.0,                               │
│    weakening: 85,                                    │
│    motorPower: 285 kW,                              │
│    travelTime: 18.5 min,                            │
│    fuzzyScore: 0.78  ← HIGH SCORE!                  │
│  }                                                   │
└─────────────────┬───────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────┐
│  Step 7: Find Optimal                               │
│  Sort by fuzzy score → Pick highest                 │
│  RESULT: Best configuration found! 🎯               │
└─────────────────────────────────────────────────────┘
```

---

## 6. Complete Membership Function Definitions

### For Implementation in Your Code

```cpp
// Acceleration membership functions
TriangleSet("Low", 0.4, 0.6, 0.8)
TriangleSet("Medium", 0.6, 1.0, 1.2)
TriangleSet("High", 1.0, 1.2, 1.4)

// Weakening Point membership functions
TriangleSet("Low", 50, 70, 85)
TriangleSet("Medium", 70, 85, 100)
TrapezoidSet("High", 85, 100, 120, 140)

// Gradient membership functions
TriangleSet("Low", 0, 5, 10)
TriangleSet("Medium", 5, 10, 20)
TriangleSet("High", 10, 30, 40)

// Speed Limit membership functions
TriangleSet("Low", 40, 60, 80)
TriangleSet("Medium", 60, 90, 110)
TrapezoidSet("High", 90, 110, 130, 140)

// Suitability (Output) membership functions
TriangleSet("NotSuitable", 0.0, 0.0, 0.3)
TriangleSet("QuiteSuitable", 0.0, 0.3, 0.5)
TriangleSet("Suitable", 0.3, 0.5, 0.7)
TrapezoidSet("VerySuitable", 0.5, 0.7, 0.85, 1.0)
```

---

## 7. Optimization Results Visualization

### Expected Output After Running 3 Iterations:

```
═══════════════════════════════════════════════════════════════
                    OPTIMIZATION RESULTS
═══════════════════════════════════════════════════════════════

Configuration 1:
  Acceleration:     1.0 m/s²
  Weakening Point:  85 km/h
  ──────────────────────────────────────────────────
  Motor Power:      285 kW/motor
  Travel Time:      18.5 min
  Energy:           245 kWh
  ──────────────────────────────────────────────────
  Fuzzy Score:      0.78 ★★★★☆
  Rating:           SUITABLE

Configuration 2:
  Acceleration:     0.8 m/s²
  Weakening Point:  85 km/h
  ──────────────────────────────────────────────────
  Motor Power:      265 kW/motor  ← CHEAPER!
  Travel Time:      19.2 min     ← Slightly slower
  Energy:           238 kWh
  ──────────────────────────────────────────────────
  Fuzzy Score:      0.82 ★★★★☆
  Rating:           VERY SUITABLE ⭐ OPTIMAL!

Configuration 3:
  Acceleration:     0.6 m/s²
  Weakening Point:  85 km/h
  ──────────────────────────────────────────────────
  Motor Power:      240 kW/motor  ← CHEAPEST
  Travel Time:      21.0 min      ← TOO SLOW
  Energy:           230 kWh
  ──────────────────────────────────────────────────
  Fuzzy Score:      0.55 ★★★☆☆
  Rating:           QUITE SUITABLE

═══════════════════════════════════════════════════════════════
                      OPTIMAL RESULT
═══════════════════════════════════════════════════════════════
  Best Configuration:  #2
  Acceleration:        0.8 m/s²
  Motor Power:         265 kW/motor
  Cost Savings:        7% less power than baseline (285 kW)
  Performance:         Only 0.7 min slower (acceptable)
  Fuzzy Score:         0.82 (VERY SUITABLE)
  
  ✅ RECOMMENDATION: Use this specification!
═══════════════════════════════════════════════════════════════
```

---

## 8. Key Insights

### Why Fuzzy Logic is Perfect for This:

1. **Handles Conflicting Goals**:
   - Low power (cheap) vs Fast travel time (expensive)
   - Fuzzy finds the BALANCE naturally

2. **Expert Knowledge Encoded**:
   - "Medium acceleration on moderate slopes is Suitable"
   - Rules capture engineering experience

3. **Smooth Transitions**:
   - No hard cutoffs (0.79 "bad", 0.80 "good")
   - Gradual membership allows nuanced evaluation

4. **Interpretable Results**:
   - Score 0.82 = "Very Suitable" 
   - Not just a number, has meaning!

---

## Summary

**Input Variables** (4):
- Acceleration ← We optimize this
- Weakening Point ← We optimize this  
- Gradient ← Fixed by track
- Speed Limit ← Fixed by track

**Output Variable** (1):
- Suitability Score (0.0 - 1.0)

**Process**:
1. Try different acceleration & weakening combinations
2. Run physics simulation for each
3. Fuzzy logic evaluates cost-performance balance
4. Pick configuration with highest fuzzy score
5. **Result**: Optimal train specification! 🎯

**Goal**: Find the cheapest motor power that still performs well!
