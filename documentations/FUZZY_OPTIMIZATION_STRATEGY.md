# Fuzzy Logic Optimization — Implementation Reference

This document explains the actual implemented system: how the fuzzy engine works
internally, how membership functions are defined and evaluated, how the inference
and defuzzification are performed, and how the two-pass optimization loop ties
everything together.

---

## 1. Architecture Overview

The optimizer has two independent layers:

```
OptimizationHandler::handleOptimization()
│
├── PASS 1: Parameter Sweep
│   ├── Build acc × v_p1 grid centred on user's current values
│   ├── For each combination → runDynamicSimulation()
│   └── Record (travelTime, peakMotorPower) per combination
│
├── PASS 2: Fuzzy Scoring
│   ├── Derive min/max ranges from actual Pass 1 results
│   ├── setupFuzzyEngine(minT, maxT, minP, maxP)   ← calibrated, not static
│   └── For each result → evaluateFuzzyScore() → OptResult.fuzzyScore
│
└── Pick best (max fuzzyScore), emit optimizationComplete(best)
```

The fuzzy engine is **never calibrated before seeing real data**. Ranges are
derived from Pass 1 output, so the same code works for any train and any track
without changing a single constant.

---

## 2. Fuzzy Engine Classes (`fuzzy_engine.h / .cpp`)

### 2.1 `FuzzySet` — Abstract Membership Function

```
FuzzySet (abstract)
│   membership(double value) → double   [0.0 – 1.0]
├── TriangleSet   (name, min, peak, max)
└── TrapezoidSet  (name, min, peak1, peak2, max)
```

### 2.2 `TriangleSet` — Membership Formula

$$
\mu(x) = \begin{cases}
0 & x \le \text{min} \text{ or } x \ge \text{max} \\
\dfrac{x - \text{min}}{\text{peak} - \text{min}} & \text{min} < x < \text{peak} \\
1 & x = \text{peak} \\
\dfrac{\text{max} - x}{\text{max} - \text{peak}} & \text{peak} < x < \text{max}
\end{cases}
$$

Used for **Medium** terms (TravelTime, MotorPower) and **Fair / Good**
output terms — shapes that peak at a midpoint and fall off symmetrically.

### 2.3 `TrapezoidSet` — Membership Formula

$$
\mu(x) = \begin{cases}
0 & x \le \text{min} \text{ or } x \ge \text{max} \\
\dfrac{x - \text{min}}{\text{peak}_1 - \text{min}} & \text{min} < x < \text{peak}_1 \\
1 & \text{peak}_1 \le x \le \text{peak}_2 \\
\dfrac{\text{max} - x}{\text{max} - \text{peak}_2} & \text{peak}_2 < x < \text{max}
\end{cases}
$$

Used for **boundary terms** (Short, Long, Low, High) where the "fully
belonging" region is a flat plateau at 1.0, not just a single point.

### 2.4 `FuzzyVariable`

Holds a list of named `FuzzySet` terms and a current crisp value `m_value`.

Key methods:

| Method                         | Purpose                                                                                |
| ------------------------------ | -------------------------------------------------------------------------------------- |
| `setValue(v)`                  | Clamps `v` to `[min, max]`, stores in `m_value`                                        |
| `getMembership(termName)`      | Returns `term->membership(m_value)` — used for antecedent evaluation                   |
| `getMembershipAt(termName, x)` | Returns `term->membership(x)` at an arbitrary point — used during centroid integration |
| `getDominantTerm()`            | Returns the term name with highest membership at current value                         |

### 2.5 `FuzzyRule`

```cpp
struct FuzzyRule {
    QMap<QString, QString> antecedents;   // varName → termName
    QPair<QString, QString> consequent;   // varName, termName
};
```

Example (Short travel time AND Low motor power → Excellent score):

```cpp
antecedents["TravelTime"] = "Short";
antecedents["MotorPower"] = "Low";
consequent = {"FuzzyScore", "Excellent"};
```

### 2.6 `FuzzyEngine`

Stores input/output variables and rules, runs inference.

```
FuzzyEngine
├── addInputVariable / addOutputVariable / addRule / clear
├── setInputValue(varName, crisp)     ← sets m_value on named FuzzyVariable
└── getOutputValue(varName)           ← full Mamdani inference + centroid defuzz
```

---

## 3. Inference Pipeline (`FuzzyEngine::getOutputValue`)

### Step 1 — Fuzzification (implicit)

`setInputValue()` stores the crisp value on each input `FuzzyVariable`.
When antecedents are evaluated, `getMembership(termName)` reads `m_value`
and computes the membership score.

### Step 2 — Rule Activation (MIN-AND)

For each rule, the activation strength is the **minimum** membership across
all its antecedents:

$$
\alpha_r = \min_{i} \, \mu_{A_i}(x_i)
$$

where $A_i$ is the antecedent term for input variable $i$ at crisp value $x_i$.

### Step 3 — Aggregation (MAX)

The output is discretised into 100 evenly-spaced points over `[min, max]`.
At each point $x$, the aggregated membership is the maximum clipped consequent
across all active rules:

$$
\mu_{\text{agg}}(x) = \max_{r \,:\, \alpha_r > 0} \min\!\left(\alpha_r,\; \mu_{B_r}(x)\right)
$$

where $\mu_{B_r}(x)$ is the raw membership of rule $r$'s consequent term
evaluated **at point $x$** (via `getMembershipAt`), not at the output variable's
stored value.

> **Why `getMembershipAt` and not `getMembership`?**
> `getMembership` reads `m_value`, which is the variable's stored crisp value.
> During defuzzification we are sweeping over $x$, not querying the stored value.
> `getMembershipAt(term, x)` passes $x$ directly to `term->membership(x)`.

### Step 4 — Defuzzification (Centroid)

$$
y^* = \frac{\displaystyle\sum_{i=0}^{100} x_i \cdot \mu_{\text{agg}}(x_i)}
           {\displaystyle\sum_{i=0}^{100} \mu_{\text{agg}}(x_i)}
$$

If the denominator is zero (no rule fired), the engine returns `0.0`.

---

## 4. Membership Functions — Exact Definitions

All input membership boundaries are computed **at runtime** using the actual
Pass 1 min/max values. A 5 % margin is added to both ends so boundary
combinations still receive non-zero membership:

```
effectiveMin = observedMin - 0.05 × range
effectiveMax = observedMax + 0.05 × range
r = effectiveMax - effectiveMin
```

### 4.1 TravelTime input

| Term   | Shape     | Parameters                          |
| ------ | --------- | ----------------------------------- |
| Short  | Trapezoid | `(min, min, min+0.25r, min+0.45r)`  |
| Medium | Triangle  | `(min+0.30r, min+0.50r, min+0.70r)` |
| Long   | Trapezoid | `(min+0.55r, min+0.75r, max, max)`  |

Short and Long overlap Medium in `[0.30r–0.45r]` and `[0.55r–0.70r]` so
intermediate travel times activate multiple terms simultaneously.

### 4.2 MotorPower input

| Term   | Shape     | Parameters                          |
| ------ | --------- | ----------------------------------- |
| Low    | Trapezoid | `(min, min, min+0.25r, min+0.45r)`  |
| Medium | Triangle  | `(min+0.30r, min+0.50r, min+0.70r)` |
| High   | Trapezoid | `(min+0.55r, min+0.75r, max, max)`  |

Identical structure to TravelTime — symmetric design.

### 4.3 FuzzyScore output (fixed range 0–100)

| Term      | Shape     | Parameters           |
| --------- | --------- | -------------------- |
| Poor      | Trapezoid | `(0, 0, 15, 30)`     |
| Fair      | Triangle  | `(20, 38, 55)`       |
| Good      | Triangle  | `(45, 62, 78)`       |
| Excellent | Trapezoid | `(68, 82, 100, 100)` |

Adjacent terms overlap (Poor/Fair: 20–30, Fair/Good: 45–55, Good/Excellent: 68–78)
so the centroid output is smooth and continuous across the 0–100 range.

---

## 5. Rule Base — 9 Mamdani Rules

| TravelTime | MotorPower | → FuzzyScore  |
| ---------- | ---------- | ------------- |
| Short      | Low        | **Excellent** |
| Short      | Medium     | Good          |
| Short      | High       | Fair          |
| Medium     | Low        | Good          |
| Medium     | Medium     | Good          |
| Medium     | High       | Fair          |
| Long       | Low        | Fair          |
| Long       | Medium     | **Poor**      |
| Long       | High       | **Poor**      |

**Design rationale:**

- Short + Low is the best case — fast trip, energy-efficient → Excellent
- Long + High is the worst case — slow and power-hungry → Poor
- The only "asymmetric" cell is Medium + Medium → Good (not Fair), reflecting
  that average performance on both metrics is still acceptable
- A fast trip with high power (Short + High) is only Fair, not Good, because
  energy efficiency is penalised even when speed is adequate

---

## 6. Two-Pass Optimization Loop

### 6.1 Sweep Grid Construction

Centred on the user's currently loaded `acc_start_si` and `v_p1`:

```
acc  : 5 values at { -0.2, -0.1, 0, +0.1, +0.2 } m/s² from acc_start_si
       clamped to [0.3, 1.5] m/s²

v_p1 : 4 offsets { -15, -5, +5, +15 } km/h from v_p1
       clamped to [20, v_limit - 5) km/h
```

Maximum grid size: 5 × 4 = 20 combinations. Values outside the physical
clamp are silently dropped; if all are dropped the original value is kept as
a single-point sweep.

### 6.2 Pass 1 — Simulation Sweep

```
for each acc in accValues:
  for each v_p1 in vp1Values:
    m_movingData->acc_start_si = acc
    m_movingData->v_p1      = v_p1
    m_trainSimulation->runDynamicSimulation()       ← acquires its own mutex
    record peakMotorPower = max(powerMotorOutPerMotor)
    record travelTime     = timeTotal.last()
```

`runDynamicSimulation()` acquires `m_simulationMutex` internally. The outer
loop holds **no mutex** — holding one here too would cause a deadlock because
`QMutex` is non-recursive.

### 6.3 Range Derivation

After Pass 1:

```
minT = min of all travelTime,   maxT = max of all travelTime
minP = min of all peakPower,    maxP = max of all peakPower

if (maxT - minT < 1.0): minT -= 1.0, maxT += 1.0   ← degenerate guard
if (maxP - minP < 1.0): minP -= 1.0, maxP += 1.0
```

The degenerate guard prevents the 5 % margin from collapsing to zero when
all simulations produce identical metrics (e.g., single-combination sweep).

### 6.4 Pass 2 — Scoring

```
setupFuzzyEngine(minT, maxT, minP, maxP)

for each RawEntry e:
  fuzzyScore = evaluateFuzzyScore(e.travelTime, e.peakPower)
  append OptResult{acc, v_p1, peakPower, travelTime, fuzzyScore}

bestResult = entry with max fuzzyScore
```

`m_resultsMutex` is held while writing to `m_results` and `m_bestResult`
so the HTTP API can safely read them from a different thread at any time.

### 6.5 Completion

```cpp
emit optimizationComplete(m_bestResult);
m_isRunning.storeRelaxed(0);
```

`m_isRunning` is a `QAtomicInt`. `testAndSetRelaxed(0, 1)` at the start of
`handleOptimization()` ensures only one run can be active at a time —
a second call while one is in progress is rejected immediately.

---

## 7. Thread Safety Summary

| Resource                            | Guard                                                              |
| ----------------------------------- | ------------------------------------------------------------------ |
| `m_results`, `m_bestResult`         | `m_resultsMutex` (QMutex)                                          |
| `m_simulationDatas`, `m_movingData` | `m_isRunning == 1` (only optimization touches these while running) |
| Simulation internals                | `m_simulationMutex` (owned by TrainSimulationHandler)              |
| Concurrent optimization start       | `m_isRunning.testAndSetRelaxed(0, 1)`                              |

---

## 8. Score Interpretation (Frontend)

| fuzzyScore | Label     | Meaning                                     |
| ---------- | --------- | ------------------------------------------- |
| ≥ 75       | Excellent | Short travel time, low motor power          |
| ≥ 50       | Good      | One metric excellent, other acceptable      |
| ≥ 25       | Fair      | Mediocre on at least one metric             |
| < 25       | Poor      | Slow trip or very high power draw (or both) |

---

## 9. Human-Friendly Explanation — What Is Fuzzy Logic Really Doing?

### The Problem With Normal "If-Else" Logic

Imagine you want to grade a student. A normal program would say:

```
if score >= 90 → A
if score >= 80 → B
if score >= 70 → C
```

A student with 79 gets C. A student with 80 gets B. One point difference, completely different grade. That feels wrong — they performed almost identically.

Fuzzy logic fixes this by allowing **partial membership**. Instead of "you are C or you are B", it says "you are 90% C and 20% B at the same time". The final answer is a weighted blend, not a hard cutoff.

---

### Applying This to Train Optimization

After running 20 simulations, each one has two results:

- **How long did the trip take?** (travel time, in seconds)
- **How hard did the motor work?** (peak motor power, in kW)

You want to find the combination that is both **fast** and **energy-efficient**. But "fast" and "efficient" are vague human words — fuzzy logic is precisely the tool for turning vague words into math.

---

### Step 1 — Turning Numbers Into Words (Fuzzification)

Say your 20 simulations produced travel times between **200 s** and **260 s**.

You divide that range into three zones: **Short**, **Medium**, **Long**. But instead of hard borders, they overlap:

```
200 s ──────────────────────────────────── 260 s
│░░░░░░░░░░Short░░░░░░│
                │░░░Medium░░░│
                         │░░░░░░░Long░░░░░░░│
```

A trip that took **215 s** falls inside the Short zone and slightly into Medium:

- Short membership: **0.89** (89% short)
- Medium membership: **0.0**
- Long membership: **0.0**

A trip that took **235 s** sits right between Medium and Long:

- Short membership: **0.0**
- Medium membership: **0.5**
- Long membership: **0.4**

This is called **fuzzification** — converting a crisp number into membership degrees for each label.

The same thing happens for motor power. Say the range is 150 kW to 200 kW. A result with 185 kW would be mostly High, somewhat Medium, not Low at all:

- Low: **0.0**
- Medium: **0.3**
- High: **0.7**

---

### Step 2 — Applying Human Judgement (The Rules)

Now you have labels for your result. You apply **if-then rules** that encode what a human engineer would say:

```
IF travel time is Short  AND motor power is Low   → score is Excellent
IF travel time is Short  AND motor power is High  → score is Fair
IF travel time is Long   AND motor power is High  → score is Poor
... (9 rules total, one for each combination)
```

These rules are just common sense written down:

- Fast trip + efficient motor = excellent
- Fast trip but motor is overloaded = only fair (speed isn't everything)
- Slow trip + inefficient motor = poor

For our example (time=215s, power=185kW → Short=0.89, High=0.7, Medium=0.3):

The engine checks every rule. For each rule, it asks: **how strongly does this result match BOTH conditions?**

It takes the **minimum** of the two memberships. Why minimum? Because AND means both must be true. If you're 0.89 short but only 0.7 high-power, you satisfy "Short AND High" only as much as the weaker side — 0.7.

```
Short AND Low   → min(0.89, 0.0) = 0.0   → Excellent rule: doesn't fire
Short AND Medium → min(0.89, 0.3) = 0.3  → Good rule: fires at 30%
Short AND High  → min(0.89, 0.7) = 0.7   → Fair rule: fires at 70%
(all other rules: at least one input is 0.0, so they don't fire)
```

Result: the Good rule fired at 30% strength, the Fair rule fired at 70% strength.

---

### Step 3 — Combining the Votes (Aggregation)

Both rules are "voting" for an output score:

- Good says the score should be around **62** (its peak), with 30% confidence
- Fair says the score should be around **38** (its peak), with 70% confidence

The engine doesn't just pick one winner. It combines both votes into a single blurry shape. Think of it like two people pushing a slider:

- One person pushes toward 62 with 30% strength
- The other pushes toward 38 with 70% strength

The shape is tall near 38 (because 0.7 confidence) and shorter near 62 (because 0.3 confidence).

---

### Step 4 — Finding One Final Number (Defuzzification)

Now we have a fuzzy output shape. We need one crisp number — the final score.

The engine finds the **centre of gravity** of that shape. Imagine the shape is a cardboard cutout — where would you balance it on your finger?

Because the shape is taller near 38 (Fair, strength 0.7) and shorter near 62 (Good, strength 0.3), the balance point is pulled toward the heavier side — landing around **45**.

**Final fuzzyScore ≈ 45** → labelled "Fair" on the frontend.

This makes intuitive sense: the trip was fast (good), but the motor was working too hard (bad). A balanced result — not great, not terrible — lands in the middle.

---

### Why Not Just Do `score = (100 - time%) + (100 - power%) / 2`?

You could. But that treats time and power as equally important and linearly related. Fuzzy logic lets you express nuance:

- Short + Medium power → **Good** (fast trip with acceptable power is rewarded more than a simple average would give)
- Medium + Medium power → **Good** (average on both is still acceptable, not just "average")
- Long + Low power → only **Fair** (being efficient doesn't save you if the trip is too slow)

The rule table encodes these non-linear trade-offs exactly as a human engineer would judge them, without needing a complex mathematical formula.

---

### Summary in One Sentence Per Step

| Step                | What Happens                                      | Human Analogy                                                       |
| ------------------- | ------------------------------------------------- | ------------------------------------------------------------------- |
| **Fuzzification**   | Convert 215 s → Short=0.89, Medium=0.0            | "This trip was mostly short, barely medium"                         |
| **Rule Evaluation** | Check all 9 IF-THEN rules, fire the matching ones | "A short+high-power trip → Fair. A short+medium-power trip → Good." |
| **Aggregation**     | Combine all fired rules into one output shape     | "Both votes combined, weighted by how strongly they fired"          |
| **Defuzzification** | Find the centre of gravity of that shape          | "Where does the overall weight of all votes land?"                  |
