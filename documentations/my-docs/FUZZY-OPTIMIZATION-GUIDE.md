# Fuzzy Optimization — Implementation Guide & Learning Roadmap

**Created**: March 3, 2026  
**Purpose**: Reference for building `optimization_handler.cpp` — what to think about and what to learn

---

## 🧱 Part 1: The LEGO Framework (What You Need)

Think of the optimization as 5 LEGO blocks you connect together.

---

### Block 1: Input Parameters

What acceleration values will you test?  
How many values? (Start simple — 3 is enough before expanding)  
What's the range?

```
acc_start  : {0.7, 0.8, 0.9, 1.0, 1.1}  m/s²  (stored in m_movingData->acc_start)
v_p1       : {55.0, 65.0, 75.0, 85.0}   km/h  (stored in m_movingData->v_p1)
```

These are the **only two knobs** being swept. Everything else stays at default.

---

### Block 2: The Simulation

You already have `TrainSimulationHandler::runDynamicSimulation()` — **blocking, synchronous**.

Before each run:
- Set `m_movingData->acc_start = acc`
- Set `m_movingData->v_p1 = vp1`

Then call `runDynamicSimulation()` — it waits until done.

The fuzzy inputs come **out** of the simulation, not in:
- **Travel Time** → `m_simulationDatas->timeTotal.last()` (seconds)
- **Motor Power per Car** → average of `m_simulationDatas->powerMotorOutPerMotor` (kW/motor)

---

### Block 3: Results Storage

After each run, remember:

```cpp
struct OptResult {
    double acc_start;      // what knob position was used
    double v_p1;           // what knob position was used
    double avgMotorPower;  // fuzzy input 1 (kW/motor)
    double travelTime;     // fuzzy input 2 (seconds)
    double fuzzyScore;     // output from fuzzy engine (0–100)
};

QList<OptResult> results;
```

Use `QList<OptResult>` — one entry per (acc, v_p1) combination.

---

### Block 4: The Fuzzy Evaluator

This is the "is this good?" judge. It takes the two simulation outputs and returns a score.

**Inputs**:
| Variable               | Low     | Medium  | High     |
| ---------------------- | ------- | ------- | -------- |
| Travel Time (s)        | 200–450 | 380–680 | 610–950  |
| Motor Power (kW/motor) | 200–560 | 490–760 | 690–1100 |

**Output**: Fuzzy Score 0–100 (Poor / Fair / Good / Excellent)

**9 Mamdani rules**:
| Time \ Power | Low       | Medium | High |
| ------------ | --------- | ------ | ---- |
| Short        | Excellent | Good   | Fair |
| Medium       | Good      | Good   | Fair |
| Long         | Fair      | Poor   | Poor |

The engine does centroid defuzzification — returns a single crisp number.

---

### Block 5: Comparison / Finding the Best

After the loop, scan all stored results and pick the one with the highest `fuzzyScore`:

```cpp
const OptResult &best = *std::max_element(
    results.begin(), results.end(),
    [](const OptResult &a, const OptResult &b) {
        return a.fuzzyScore < b.fuzzyScore;
    });
```

Print: `acc_start`, `v_p1`, `avgMotorPower`, `travelTime`, `fuzzyScore`.

---

### The Logic Flow (Step by Step)

```
START
  │
  ▼
Setup fuzzy engine (membership functions + rules)
  │
  ▼
For each acc_start in {0.7, 0.8, 0.9, 1.0, 1.1}:
  For each v_p1 in {55, 65, 75, 85}:
    │
    ├─ A. Set m_movingData->acc_start and v_p1
    ├─ B. runDynamicSimulation()
    ├─ C. Read timeTotal.last() + avg(powerMotorOutPerMotor)
    ├─ D. evaluateFuzzyScore(time, power) → score
    ├─ E. Store OptResult in QList
    └─ F. .clear() powerMotorOutPerMotor   ← CRITICAL for 4GB RAM
  │
  ▼
Find max fuzzyScore in QList
  │
  ▼
Print winner
  │
  ▼
END
```

---

### Questions to Answer Before Touching Code

| Question                               | Answer                                                               |
| -------------------------------------- | -------------------------------------------------------------------- |
| Where is acceleration stored?          | `m_movingData->acc_start`                                            |
| Where is field weakening stored?       | `m_movingData->v_p1`                                                 |
| Where is motor power after simulation? | `m_simulationDatas->powerMotorOutPerMotor` (QList)                   |
| Where is travel time after simulation? | `m_simulationDatas->timeTotal.last()`                                |
| What makes a result "best"?            | Highest fuzzy score (balance of low power + short time)              |
| Why `.clear()` between runs?           | Same QList gets appended each run — grows unbounded without clearing |
| How many simulation runs total?        | 5 × 4 = 20 (coarse sweep)                                            |

---

### The 4 Implementation Phases

**Phase 1 — One run**  
Set acc to 1.0, run simulation, print motor power to console.  
Goal: Confirm you can extract the power value.

**Phase 2 — Three runs**  
Loop `{0.8, 1.0, 1.2}`, print results for each.  
Goal: See if different acc → different power.

**Phase 3 — Store results**  
Use `OptResult` struct + `QList`, save each result.  
Goal: Have data to compare.

**Phase 4 — Find best**  
Compare stored results, print the winner.  
Goal: Complete optimization.

---

## 📚 Part 2: Learning Roadmap

---

### 🔴 MUST LEARN (Essential)

#### 1. Coarse-to-Fine Search Strategy

Like finding a book in a library:
- **Coarse**: Check which floor/section (fast, rough)
- **Fine**: Check exact shelf (slow, precise)

In practice:
```
Coarse: test acc {0.7, 0.9, 1.1} → find 0.9 is best region
Fine:   test acc {0.85, 0.88, 0.90, 0.92, 0.95} around 0.9 → find 0.92 is optimal
```

Why: Full brute-force = ~1,200 runs. Coarse-to-fine = ~150 runs. Same result, 8× faster.

**Learn**: Grid search, parameter tuning basics (1–2 hours reading).

---

#### 2. Multi-dimensional Parameter Space

Your parameters create a "space" of possibilities — like adjusting 5 sliders on an equalizer. Each combination produces a different simulation result.

What to learn:
- How to iterate through combinations → **nested loops**
- How to store results → **`std::vector<struct>` or `QList<struct>`**

**Learn**: Basic C++ STL containers — vectors, structs (2–3 hours).

---

#### 3. Memory Management Basics (4GB RAM Constraint)

Full simulation ≈ 100KB data. 150 runs × 100KB = 15MB. On a 4GB machine with OS + Qt + WebEngine running, free RAM may be only 1–2GB.

Key rules:
- **Always `.clear()` per-run lists after reading them** — `powerMotorOutPerMotor`, etc.
- Stack variables clean themselves. Heap needs manual management.
- Qt's `QList` is heap-backed — it grows until you clear it.

**Learn**: C++ memory management, RAII pattern, smart pointers (3–4 hours).

---

### 🟡 GOOD TO LEARN (Makes it Easier)

#### 4. Parallel Processing Basics

Run 2 simulations simultaneously instead of sequentially.

Challenge: Both share the same `trainData`, `movingData`, etc.  
Solution: Thread safety with `QMutex` — already in your code (`m_simulationMutex`).

What to know:
- Thread vs Process
- Race conditions (why mutex is needed)
- `QtConcurrent::run()` (already used in your codebase)

**Learn**: Basic multithreading, Qt Concurrent (2–3 hours).

---

#### 5. Sorting and Ranking with Lambdas

After 20 coarse runs, find the top 3 best scores:

```cpp
std::sort(results.begin(), results.end(),
    [](const OptResult &a, const OptResult &b) {
        return a.fuzzyScore > b.fuzzyScore; // descending
    });
// results[0] is now the winner
```

**Learn**: Lambda functions, `std::sort`, comparators (1–2 hours).

---

### 🟢 NICE TO HAVE (Advanced)

#### 6. Algorithm Complexity (Big O Notation)

Why coarse-to-fine beats full grid search:
- Full grid: O(n⁵) where n = values per parameter
- Coarse-to-fine: O(3n³) — significantly faster

**Learn**: Basic algorithm analysis (1–2 hours, optional).

---

#### 7. Profiling and Performance Measurement

Tools to measure where your code is slow:
- `QElapsedTimer` — time specific sections
- Valgrind — memory profiling (Linux)
- Qt Creator's built-in profiler

**Learn**: Performance profiling basics (2–3 hours, optional).

---

### 🗓️ Practical Learning Path

| Days    | Focus                                                       |
| ------- | ----------------------------------------------------------- |
| Day 1–2 | C++ vectors, structs, loops — practice with simple examples |
| Day 3–4 | Memory management — understand simulation data lifecycle    |
| Day 5   | Coarse-to-fine — draw diagrams, simulate on paper           |
| Day 6–7 | Implement Phase 1 & 2 (single run → three runs)             |
| Day 8–9 | Implement Phase 3 & 4 (store → find best)                   |
| Day 10  | Test, debug, measure memory                                 |

---

### Resources

| Topic          | Resource                                                                                  |
| -------------- | ----------------------------------------------------------------------------------------- |
| Coarse-to-fine | Search: "hyperparameter tuning grid search"; YouTube: "Parameter Optimization Strategies" |
| C++ memory     | learncpp.com memory management chapter; cppreference.com smart pointers                   |
| Qt specifics   | Qt docs: `QMutex`, `QtConcurrent`; your own `train_simulation_handler.cpp`                |

---

_"Don't build everything at once. Phase 1 first. One run. Print the number. Then grow."_
