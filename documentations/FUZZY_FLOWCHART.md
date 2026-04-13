# Fuzzy Logic Optimization — Flowcharts (Plain Language)

---

## Flowchart 1 — The Big Picture: How the Optimizer Finds the Best Setting

Think of this like a cooking competition. The chef (optimizer) tries 20 different recipes (parameter combinations), then a judge (fuzzy engine) scores each dish and picks the winner.

```mermaid
flowchart TD
    A([User opens the Optimization page]) --> B{Has the user run\na simulation at least once?}
    B -- No --> C[/Tell the user:\nPlease run a simulation first\nso we have a starting point/]
    C --> Z([Done])

    B -- Yes --> D[Remember the user's current settings:\nacc_start_si and v_p1]

    D --> E["Build 20 combinations to try:\n• 5 slightly different acceleration values\n  centred on the current one\n• 4 slightly different field-weakening speeds\n  centred on the current one\n  = 5 × 4 = 20 combinations"]

    E --> F[/"=== ROUND 1 — Try all 20 combinations ===\nFor each combination, run a full simulation\nand record two numbers:\n1. How long did the trip take?\n2. How hard did the motor work?"/]

    F --> G{Did all simulations finish\nwithout errors?}
    G -- Some failed --> H[Skip the failed ones,\ncontinue with the rest]
    G -- All failed --> I[/Show error to user/]
    I --> Z
    H --> J
    G -- All ok --> J

    J[Now we have up to 20 results,\neach with a travel time and a peak power]

    J --> K["Look at ALL the results together:\n• Which result was fastest? Which was slowest?\n• Which used the least power? The most?\nThis tells us what 'fast' and 'slow' actually mean\nfor THIS train on THIS track"]

    K --> L[/"=== ROUND 2 — Score each result ===\nFeed each result into the Fuzzy Judge\nsee Flowchart 2 for how the judge works"/]

    L --> M[Every result now has a score from 0 to 100]

    M --> N[Pick the result with the highest score]

    N --> O[/Show the winner and the full ranking\nto the user/]
    O --> Z
```

---

## Flowchart 2 — The Fuzzy Judge: How One Result Gets a Score

Imagine a judge at a train performance competition. They look at two things: **was the trip fast?** and **was the motor not overworked?** They don't give binary pass/fail — they give partial credit, like a human would.

```mermaid
flowchart TD
    A(["One result arrives:\ntrip took 215 seconds\nmotor peaked at 185 kW"]) --> B

    B["QUESTION 1: How fast was this trip?\nCompare 215 s against all other results.\nThe judge asks: is 215 s short, medium, or long\nCOMPARED TO THE OTHER 19 results?"]

    B --> C{"Where does 215 s sit\nin the range of all results?"}
    C --> C1["It is near the fast end\n→ Mostly SHORT\ne.g. Short = 0.89, Medium = 0.0, Long = 0.0"]
    C --> C2["It is in the middle\n→ Partly SHORT, partly MEDIUM\ne.g. Short = 0.5, Medium = 0.4, Long = 0.0"]
    C --> C3["It is near the slow end\n→ Mostly LONG\ne.g. Short = 0.0, Medium = 0.3, Long = 0.8"]

    C1 --> D
    C2 --> D
    C3 --> D

    D["QUESTION 2: How hard did the motor work?\nSame idea — compare 185 kW against all results.\nIs 185 kW low, medium, or high power?"]

    D --> E{"Where does 185 kW sit\nin the range of all results?"}
    E --> E1["Near the low end → Mostly LOW\ne.g. Low = 0.9, Medium = 0.1, High = 0.0"]
    E --> E2["In the middle → Mixed\ne.g. Low = 0.4, Medium = 0.5, High = 0.1"]
    E --> E3["Near the high end → Mostly HIGH\ne.g. Low = 0.0, Medium = 0.3, High = 0.7"]

    E1 --> F
    E2 --> F
    E3 --> F

    F["APPLY THE JUDGE'S RULES\nThe judge has 9 opinion rules — see Flowchart 3\nEach rule that matches this result\ngives a partial vote for a score label"]

    F --> G["COMBINE ALL THE VOTES\nExample:\n• Fair got 70% votes → pulls score toward ~38\n• Good got 30% votes → pulls score toward ~62\nThe final score lands at the weighted middle"]

    G --> H(["Final score for this result:\ne.g. fuzzyScore = 45 → labelled Fair"])
```

---

## Flowchart 3 — The Judge's 9 Opinion Rules

This is the human knowledge baked into the system. A human engineer wrote these rules once. The engine applies them to every result automatically.

```mermaid
flowchart TD
    A(["For each result:\nwe know if it is Short/Medium/Long time\nand Low/Medium/High power"])

    A --> B{"Trip time?"}

    B -- "Mostly SHORT\nfast trip" --> S1
    B -- "Mostly MEDIUM\naverage trip" --> M1
    B -- "Mostly LONG\nslow trip" --> L1

    subgraph SR ["If the trip was SHORT (fast)..."]
        S1{"Motor power?"}
        S1 -- Low --> SA["Score: EXCELLENT\nFast AND efficient — best case"]
        S1 -- Medium --> SB["Score: GOOD\nFast, motor worked a normal amount"]
        S1 -- High --> SC["Score: FAIR\nFast, but motor was overloaded"]
    end

    subgraph MR ["If the trip was MEDIUM (average)..."]
        M1{"Motor power?"}
        M1 -- Low --> MA["Score: GOOD\nNot the fastest but very efficient"]
        M1 -- Medium --> MB["Score: GOOD\nAverage on both — still acceptable"]
        M1 -- High --> MC["Score: FAIR\nAverage speed but high power cost"]
    end

    subgraph LR ["If the trip was LONG (slow)..."]
        L1{"Motor power?"}
        L1 -- Low --> LA["Score: FAIR\nEfficient, but the trip was too slow"]
        L1 -- Medium --> LB["Score: POOR\nSlow trip, average power — not good"]
        L1 -- High --> LC["Score: POOR\nSlow AND power-hungry — worst case"]
    end

    SA & SB & SC & MA & MB & MC & LA & LB & LC --> END

    END["Each matching rule casts a partial vote\nbased on how strongly the result\nmatched that rule's conditions\ne.g. if Short=0.89 and High=0.7\nthe 'Short AND High → Fair' rule\nvotes with strength 0.7\nAll votes are combined into one final score"]
```

---

## How to View These Diagrams

### Option 1 — VS Code (recommended, free)

1. Press `Ctrl+Shift+X`
2. Search **Markdown Preview Mermaid Support** (by Matt Bierner) → Install
3. Open this file → press `Ctrl+Shift+V`
4. The flowcharts render live inside the preview panel

### Option 2 — Online (best for exporting to PNG/PDF for your thesis)

1. Go to **https://mermaid.live**
2. Copy the code between the triple backticks of any flowchart
3. Paste it into the left panel — the diagram appears on the right
4. Click **Export** → PNG or SVG

### Option 3 — GitHub

Push this file and open it on GitHub. Mermaid renders automatically — no tools needed.

```mermaid
flowchart TD
    A([Start]) --> B[/User clicks Start Optimization/]
    B --> C{Has the user run\na simulation first?}
    C -- No --> D[/Show error:\nRun a simulation first/]
    D --> Z([End])
    C -- Yes --> E[Save user's original\nacc_start_si and v_p1]

    E --> F[Build candidate lists:\nacc: 5 values centred on current acc\nv_p1: 4 offsets from current v_p1]

    F --> G[/=== PASS 1: Simulation Sweep ===/]

    G --> H[Take next acc × v_p1 combination]
    H --> I[Set acc_start_si and v_p1\nin simulation parameters]
    I --> J[Run dynamic simulation]
    J --> K{Did simulation\nproduce output?}
    K -- No --> L[Skip this combination\nlog warning]
    K -- Yes --> M[Record:\n- travel time = last timestamp\n- peak power = max motor power]
    M --> N{More combinations\nremaining?}
    L --> N
    N -- Yes --> H
    N -- No --> O[Restore user's original\nacc_start_si and v_p1]

    O --> P{Any valid\nresults from Pass 1?}
    P -- No --> Q[/Show error:\nNo results produced/]
    Q --> Z
    P -- Yes --> R[Find min and max of\nall travel times and powers]

    R --> S[Add 5% margin to\nboth ends of each range]
    S --> T[Build fuzzy membership functions\nusing actual data ranges]

    T --> U[/=== PASS 2: Fuzzy Scoring ===/]

    U --> V[Take next result from Pass 1]
    V --> W[Run fuzzy scoring\nsee Flowchart 2]
    W --> X[Store fuzzyScore\nwith the result]
    X --> Y{More results\nto score?}
    Y -- Yes --> V
    Y -- No --> AA[Find result with\nhighest fuzzyScore]

    AA --> BB[/Emit optimizationComplete\nwith best result/]
    BB --> CC[/Frontend shows\nwinner card + results table/]
    CC --> Z
```

---

## Flowchart 2 — Fuzzy Scoring (one result)

This runs once per result in Pass 2. Input: one `(travelTime, peakPower)` pair.

```mermaid
flowchart TD
    A([Input:\ntravelTime, peakPower]) --> B

    subgraph FUZZ ["Step 1 — Fuzzification"]
        B[Calculate TravelTime memberships:\nShort = ? Medium = ? Long = ?]
        C[Calculate MotorPower memberships:\nLow = ? Medium = ? High = ?]
    end

    B --> C
    C --> D

    subgraph RULES ["Step 2 — Rule Evaluation × 9 rules"]
        D[Rule 1: Short AND Low → Excellent\nactivation = min Short, Low]
        E[Rule 2: Short AND Medium → Good\nactivation = min Short, Medium]
        F[Rule 3: Short AND High → Fair\nactivation = min Short, High]
        G[Rule 4: Medium AND Low → Good\nactivation = min Medium, Low]
        H[Rule 5: Medium AND Medium → Good\nactivation = min Medium, Medium]
        I[Rule 6: Medium AND High → Fair\nactivation = min Medium, High]
        J[Rule 7: Long AND Low → Fair\nactivation = min Long, Low]
        K[Rule 8: Long AND Medium → Poor\nactivation = min Long, Medium]
        L[Rule 9: Long AND High → Poor\nactivation = min Long, High]
    end

    D --> M
    E --> M
    F --> M
    G --> M
    H --> M
    I --> M
    J --> M
    K --> M
    L --> M

    subgraph AGG ["Step 3 — Aggregation"]
        M[For each point x from 0 to 100:\nCollect all fired rules that map to x\nTake MAX of all clipped memberships at x\nThis builds the output shape]
    end

    M --> N

    subgraph DEFUZZ ["Step 4 — Defuzzification"]
        N[Find the centre of gravity\nof the output shape:\nscore = Σ x × shape_x  ÷  Σ shape_x]
    end

    N --> O([Output:\nfuzzyScore between 0 and 100])
```

---

---

## How to View These Diagrams

### Option 1 — VS Code (recommended)

Install the extension **"Markdown Preview Mermaid Support"** by Matt Bierner:

- Press `Ctrl+Shift+X` → search `Markdown Preview Mermaid Support` → Install
- Open this file → press `Ctrl+Shift+V` to open the Markdown preview
- The flowcharts render automatically inside the preview

### Option 2 — GitHub

Push this file to your repository. GitHub renders Mermaid diagrams natively inside Markdown files — no extension needed.

### Option 3 — Online

Copy any code block (the text between the triple backticks) and paste it into **https://mermaid.live** — it renders and you can export as PNG or SVG for your thesis.
