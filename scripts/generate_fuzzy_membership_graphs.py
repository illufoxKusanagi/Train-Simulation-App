#!/usr/bin/env python3
"""
Fuzzy Membership Function Visualizer — Train Optimization
Mirrors the exact C++ implementation in backend/utils/ and
backend/controllers/optimization/optimization_handler.cpp

TWO MAMDANI ENGINES:
  m_timeEngine  : TravelTime  → TimeScore   (Short/Medium/Long  → Excellent/Good/Poor)
  m_powerEngine : MotorPower  → PowerScore  (Low/Medium/High    → Excellent/Good/Poor)
  Final score   = (TimeScore + PowerScore) / 2.0

Input terms use DYNAMIC breakpoints (5% margin + percentage fractions of value range).
Output terms are FIXED on [0, 100]: Poor, Fair, Good, Excellent.
Defuzzification: Centre of Gravity (COG), 101-point discrete approximation.

GOOGLE COLAB USAGE:
------------------
1. Copy this entire file into a Colab cell and run it (defines all functions).
2. In a new cell, call:

   main()                                         # show inline, save files
   main(display_inline=True,  save_files=False)   # show only (no files)
   main(display_inline=False, save_files=True)    # save only (no popup)

   # Custom train data ranges (seconds, kW):
   main(min_time=300, max_time=800, min_power=150, max_power=600)

3. Download: Files panel (left sidebar) → fuzzy_graphs/ → right-click → Download

LOCAL USAGE:
-----------
   python3 generate_fuzzy_membership_graphs.py
"""

import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.lines import Line2D


# =============================================================================
# ── SECTION 1: Membership Function Primitives (exact mirror of C++) ──────────
# =============================================================================

def trapezoid_mf(x, m_min, m_peak1, m_peak2, m_max):
    """
    Mirrors TrapezoidSet::membership() in trapezoid_set.cpp exactly.

    Parameters (matching C++ constructor order):
      m_min   : left foot  — membership rises from here
      m_peak1 : left shoulder  — membership = 1.0 from here
      m_peak2 : right shoulder — membership = 1.0 until here
      m_max   : right foot — membership falls to 0 here

    Edge rule: value == m_min OR value == m_max → returns 0.0
    (strict inequalities in the C++ source)
    """
    x = np.asarray(x, dtype=float)
    result = np.zeros_like(x)

    # Rising slope
    rise_mask = (x > m_min) & (x < m_peak1)
    if m_peak1 != m_min:
        result[rise_mask] = (x[rise_mask] - m_min) / (m_peak1 - m_min)

    # Flat top
    flat_mask = (x >= m_peak1) & (x <= m_peak2)
    result[flat_mask] = 1.0

    # Falling slope
    fall_mask = (x > m_peak2) & (x < m_max)
    if m_max != m_peak2:
        result[fall_mask] = (m_max - x[fall_mask]) / (m_max - m_peak2)

    return result


def triangle_mf(x, m_min, m_peak, m_max):
    """
    Mirrors TriangleSet::membership() in triangle_set.cpp exactly.

    Parameters (matching C++ constructor order):
      m_min  : left foot  — membership = 0
      m_peak : apex       — membership = 1
      m_max  : right foot — membership = 0

    Edge rule: value == m_min OR value == m_max → returns 0.0
    """
    x = np.asarray(x, dtype=float)
    result = np.zeros_like(x)

    rise_mask = (x > m_min) & (x < m_peak)
    if m_peak != m_min:
        result[rise_mask] = (x[rise_mask] - m_min) / (m_peak - m_min)

    peak_mask = (x == m_peak)
    result[peak_mask] = 1.0

    fall_mask = (x > m_peak) & (x < m_max)
    if m_max != m_peak:
        result[fall_mask] = (m_max - x[fall_mask]) / (m_max - m_peak)

    return result


# =============================================================================
# ── SECTION 2: Engine Setup (mirrors setupTimeEngine / setupPowerEngine) ─────
# =============================================================================

def compute_input_breakpoints(raw_min, raw_max):
    """
    Mirrors the breakpoint calculation at the top of both setup functions:

        margin    = (maxT - minT) * 0.05
        minT     -= margin
        maxT     += margin
        vr        = maxT - minT

        Short/Low : Trapezoid(minT, minT,             minT+0.25*vr, minT+0.45*vr)
        Medium    : Triangle( minT+0.30*vr,           minT+0.50*vr, minT+0.70*vr)
        Long/High : Trapezoid(minT+0.55*vr,           minT+0.75*vr, maxT,         maxT)

    Returns a dict with keys 'min', 'max', and per-term parameter tuples.
    """
    margin = (raw_max - raw_min) * 0.05
    lo = raw_min - margin
    hi = raw_max + margin
    vr = hi - lo

    return {
        'min': lo,
        'max': hi,
        'Short_Low': ('trapezoid', lo,              lo,              lo + 0.25 * vr, lo + 0.45 * vr),
        'Medium':    ('triangle',  lo + 0.30 * vr,  lo + 0.50 * vr,  lo + 0.70 * vr),
        'Long_High': ('trapezoid', lo + 0.55 * vr,  lo + 0.75 * vr,  hi,             hi),
    }


# Output terms are FIXED on [0, 100] — same for both TimeScore and PowerScore.
OUTPUT_TERMS = {
    'Poor':      ('trapezoid',  0.0,  0.0, 15.0, 30.0),
    'Fair':      ('triangle',  20.0, 38.0, 55.0),
    'Good':      ('triangle',  45.0, 62.0, 78.0),
    'Excellent': ('trapezoid', 68.0, 82.0, 100.0, 100.0),
}

OUTPUT_COLORS = {
    'Poor': 'red',
    'Fair': 'orange',
    'Good': 'yellowgreen',
    'Excellent': 'darkgreen',
}

INPUT_COLORS = {
    'Short_Low': 'darkgreen',
    'Medium':    'orange',
    'Long_High': 'crimson',
}


def eval_term(params, x):
    """Evaluate a term from an OUTPUT_TERMS or compute_input_breakpoints entry."""
    kind = params[0]
    if kind == 'trapezoid':
        return trapezoid_mf(x, *params[1:])
    return triangle_mf(x, *params[1:])


# =============================================================================
# ── SECTION 3: COG Defuzzification (mirrors getOutputValue in C++) ───────────
# =============================================================================

def cog_defuzzify(input_value, input_bp, input_term_names, rules, label='Score'):
    """
    Runs a full single-input Mamdani inference + COG defuzzification.

    Mirrors FuzzyEngine::getOutputValue() exactly:
      - Activation = membership of input term at input_value (MIN with 1 antecedent)
      - Implication = MIN(activation, output_term_membership)   [Mamdani clip]
      - Aggregation = MAX over all active rules                  [MAX aggregation]
      - Defuzzification = COG over 101 points on [0, 100]

    Parameters
    ----------
    input_value   : crisp input (seconds or kW)
    input_bp      : result of compute_input_breakpoints(raw_min, raw_max)
    input_term_names : ['Short_Low', 'Medium', 'Long_High'] or ['Short_Low',...]
    rules         : list of (input_term_key, output_term_name) pairs
    label         : name used in print output

    Returns
    -------
    cog  : defuzzified score
    xs   : 101 sample points on [0, 100]
    agg  : aggregated membership array at each xs point
    acts : dict {output_term_name: activation_strength}
    """
    # Step 1 — Fuzzify input
    activations = {}
    for in_term, out_term in rules:
        params = input_bp[in_term]
        act = float(eval_term(params, np.array([input_value]))[0])
        activations[(in_term, out_term)] = act

    # Step 2 — Discretize output [0, 100] with 101 points (mirrors 100 steps in C++)
    xs = np.linspace(0, 100, 101)

    # Step 3 — Aggregate: MAX over all clipped output terms
    agg = np.zeros_like(xs)
    for (in_term, out_term), act in activations.items():
        if act <= 0.0:
            continue
        out_params = OUTPUT_TERMS[out_term]
        out_mf = eval_term(out_params, xs)
        clipped = np.minimum(act, out_mf)   # Mamdani MIN implication
        agg = np.maximum(agg, clipped)      # MAX aggregation

    # Step 4 — Centroid
    denom = np.sum(agg)
    cog = float(np.sum(xs * agg) / denom) if denom > 0 else 0.0

    # Build per-term activation dict for display
    acts = {out_term: activations.get((in_term, out_term), 0.0)
            for in_term, out_term in rules}

    return cog, xs, agg, acts


# =============================================================================
# ── SECTION 4: Individual Plot Functions ─────────────────────────────────────
# =============================================================================

INPUT_LABEL_MAP = {
    'Short_Low': 'Short',
    'Medium':    'Medium',
    'Long_High': 'Long',
}

POWER_LABEL_MAP = {
    'Short_Low': 'Low',
    'Medium':    'Medium',
    'Long_High': 'High',
}


def _plot_input_mf(ax, bp, label_map, xlabel, title, example_val=None):
    """Shared helper for plotting a 3-term input variable."""
    lo, hi = bp['min'], bp['max']
    pad = (hi - lo) * 0.05
    x = np.linspace(lo - pad, hi + pad, 2000)

    for key, color in INPUT_COLORS.items():
        mf = eval_term(bp[key], x)
        label = label_map[key]
        ax.fill_between(x, 0, mf, alpha=0.20, color=color)
        ax.plot(x, mf, color=color, linewidth=2.5, label=label)

    if example_val is not None:
        ax.axvline(x=example_val, color='navy', linestyle='--', linewidth=1.8,
                   label=f'Input = {example_val:.1f}')
        for key, color in INPUT_COLORS.items():
            deg = float(eval_term(bp[key], np.array([example_val]))[0])
            label = label_map[key]
            ax.plot(example_val, deg, 'o', color=color, markersize=10,
                    zorder=5, label=f'{label} = {deg:.3f}')

    ax.set_xlabel(xlabel, fontsize=11, fontweight='bold')
    ax.set_ylabel('Membership Degree', fontsize=11, fontweight='bold')
    ax.set_title(title, fontsize=12, fontweight='bold')
    ax.legend(fontsize=9, loc='upper right')
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.set_xlim(lo - pad, hi + pad)
    ax.set_ylim(-0.05, 1.15)


def _plot_output_mf(ax, title, example_x=None, agg=None, xs=None, cog=None):
    """Shared helper for plotting the output (TimeScore / PowerScore) MFs."""
    x = np.linspace(0, 100, 2000)

    for term, params in OUTPUT_TERMS.items():
        color = OUTPUT_COLORS[term]
        mf = eval_term(params, x)
        ax.fill_between(x, 0, mf, alpha=0.20, color=color)
        ax.plot(x, mf, color=color, linewidth=2.5, label=term)

    if agg is not None and xs is not None:
        ax.fill_between(xs, 0, agg, alpha=0.35, color='steelblue',
                        label='Aggregated (clipped)')
        ax.plot(xs, agg, color='steelblue', linewidth=1.5, linestyle='-')

    if cog is not None:
        ax.axvline(x=cog, color='black', linestyle='-', linewidth=2.2,
                   label=f'COG = {cog:.2f}')

    ax.set_xlabel('Score (0–100)', fontsize=11, fontweight='bold')
    ax.set_ylabel('Membership Degree', fontsize=11, fontweight='bold')
    ax.set_title(title, fontsize=12, fontweight='bold')
    ax.legend(fontsize=9, loc='upper right')
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.set_xlim(-2, 102)
    ax.set_ylim(-0.05, 1.15)


def plot_time_engine(raw_min_t, raw_max_t, example_t=None, display_inline=True, save_files=True, output_dir='fuzzy_graphs'):
    """
    Plot the full m_timeEngine:
      Row 1 — TravelTime input MFs  (+ optional fuzzification example)
      Row 2 — TimeScore output MFs  (+ COG result when example given)
    """
    bp = compute_input_breakpoints(raw_min_t, raw_max_t)
    rules = [('Short_Low', 'Excellent'), ('Medium', 'Good'), ('Long_High', 'Poor')]

    cog = xs = agg = acts = None
    if example_t is not None:
        cog, xs, agg, acts = cog_defuzzify(example_t, bp, ['Short_Low', 'Medium', 'Long_High'], rules, 'TimeScore')
        print(f"\n  [Time Engine] Input = {example_t:.1f} s")
        for out_term, act in acts.items():
            print(f"    → {out_term:9s} activation = {act:.4f}")
        print(f"    COG TimeScore = {cog:.3f} / 100")

    fig, axes = plt.subplots(1, 2, figsize=(14, 5))
    fig.suptitle('m_timeEngine — TravelTime → TimeScore  (Mamdani + COG)',
                 fontsize=13, fontweight='bold')

    _plot_input_mf(axes[0], bp, INPUT_LABEL_MAP,
                   f'Travel Time (s)  [raw range: {raw_min_t:.0f}–{raw_max_t:.0f} s]',
                   'Input: TravelTime  (Short / Medium / Long)',
                   example_val=example_t)

    title_out = f'Output: TimeScore — COG = {cog:.2f}' if cog is not None else 'Output: TimeScore'
    _plot_output_mf(axes[1], title_out, agg=agg, xs=xs, cog=cog)

    plt.tight_layout()
    if save_files:
        fig.savefig(os.path.join(output_dir, 'time_engine.png'), dpi=300, bbox_inches='tight')
        fig.savefig(os.path.join(output_dir, 'time_engine.pdf'), bbox_inches='tight')
    if display_inline:
        plt.show()
    else:
        plt.close(fig)
    return fig


def plot_power_engine(raw_min_p, raw_max_p, example_p=None, display_inline=True, save_files=True, output_dir='fuzzy_graphs'):
    """
    Plot the full m_powerEngine:
      Left  — MotorPower input MFs  (+ optional fuzzification example)
      Right — PowerScore output MFs (+ COG result when example given)
    """
    bp = compute_input_breakpoints(raw_min_p, raw_max_p)
    rules = [('Short_Low', 'Excellent'), ('Medium', 'Good'), ('Long_High', 'Poor')]

    cog = xs = agg = acts = None
    if example_p is not None:
        cog, xs, agg, acts = cog_defuzzify(example_p, bp, ['Short_Low', 'Medium', 'Long_High'], rules, 'PowerScore')
        print(f"\n  [Power Engine] Input = {example_p:.1f} kW")
        for out_term, act in acts.items():
            print(f"    → {out_term:9s} activation = {act:.4f}")
        print(f"    COG PowerScore = {cog:.3f} / 100")

    fig, axes = plt.subplots(1, 2, figsize=(14, 5))
    fig.suptitle('m_powerEngine — MotorPower → PowerScore  (Mamdani + COG)',
                 fontsize=13, fontweight='bold')

    _plot_input_mf(axes[0], bp, POWER_LABEL_MAP,
                   f'Motor Power (kW)  [raw range: {raw_min_p:.0f}–{raw_max_p:.0f} kW]',
                   'Input: MotorPower  (Low / Medium / High)',
                   example_val=example_p)

    title_out = f'Output: PowerScore — COG = {cog:.2f}' if cog is not None else 'Output: PowerScore'
    _plot_output_mf(axes[1], title_out, agg=agg, xs=xs, cog=cog)

    plt.tight_layout()
    if save_files:
        fig.savefig(os.path.join(output_dir, 'power_engine.png'), dpi=300, bbox_inches='tight')
        fig.savefig(os.path.join(output_dir, 'power_engine.pdf'), bbox_inches='tight')
    if display_inline:
        plt.show()
    else:
        plt.close(fig)
    return fig


def plot_output_score_mf(display_inline=True, save_files=True, output_dir='fuzzy_graphs'):
    """
    Standalone plot of the shared output variable MFs (TimeScore / PowerScore).
    These are identical for both engines — fixed on [0, 100].
    """
    fig, ax = plt.subplots(figsize=(10, 5))
    _plot_output_mf(ax, 'Output Score MFs — shared by both engines\n(TimeScore and PowerScore use identical terms)')
    plt.tight_layout()
    if save_files:
        fig.savefig(os.path.join(output_dir, 'output_score_mf.png'), dpi=300, bbox_inches='tight')
        fig.savefig(os.path.join(output_dir, 'output_score_mf.pdf'), bbox_inches='tight')
    if display_inline:
        plt.show()
    else:
        plt.close(fig)
    return fig


def plot_final_score_sweep(raw_min_t, raw_max_t, raw_min_p, raw_max_p,
                           display_inline=True, save_files=True, output_dir='fuzzy_graphs'):
    """
    Sweep all 20 (acc_start_si × v_p1) candidate combinations that Pass 2 would
    evaluate, computing timeScore, powerScore, and finalScore.

    Uses 4 × 5 = 20 evenly-spaced points inside the ranges as a demo —
    replace with real simulation data if available.
    """
    bp_t = compute_input_breakpoints(raw_min_t, raw_max_t)
    bp_p = compute_input_breakpoints(raw_min_p, raw_max_p)
    rules = [('Short_Low', 'Excellent'), ('Medium', 'Good'), ('Long_High', 'Poor')]

    times  = np.linspace(raw_min_t, raw_max_t, 5)
    powers = np.linspace(raw_min_p, raw_max_p, 4)

    results = []
    for t in times:
        for p in powers:
            ts, _, _, _ = cog_defuzzify(t,  bp_t, ['Short_Low', 'Medium', 'Long_High'], rules)
            ps, _, _, _ = cog_defuzzify(p,  bp_p, ['Short_Low', 'Medium', 'Long_High'], rules)
            fs = (ts + ps) / 2.0
            results.append((t, p, ts, ps, fs))

    times_all  = [r[0] for r in results]
    powers_all = [r[1] for r in results]
    final_all  = [r[4] for r in results]

    fig, axes = plt.subplots(1, 3, figsize=(16, 5))
    fig.suptitle('Final Score = (TimeScore + PowerScore) / 2   [20-combo sweep demo]',
                 fontsize=12, fontweight='bold')

    sc0 = axes[0].scatter(times_all, [r[2] for r in results], c=final_all,
                          cmap='RdYlGn', s=80, vmin=0, vmax=100)
    axes[0].set_xlabel('Travel Time (s)'); axes[0].set_ylabel('TimeScore')
    axes[0].set_title('TimeScore vs Travel Time'); axes[0].grid(True, alpha=0.3)

    sc1 = axes[1].scatter(powers_all, [r[3] for r in results], c=final_all,
                          cmap='RdYlGn', s=80, vmin=0, vmax=100)
    axes[1].set_xlabel('Motor Power (kW)'); axes[1].set_ylabel('PowerScore')
    axes[1].set_title('PowerScore vs Motor Power'); axes[1].grid(True, alpha=0.3)

    sc2 = axes[2].scatter(times_all, powers_all, c=final_all,
                          cmap='RdYlGn', s=120, vmin=0, vmax=100)
    axes[2].set_xlabel('Travel Time (s)'); axes[2].set_ylabel('Motor Power (kW)')
    axes[2].set_title('Final Score (colour) in Input Space')
    axes[2].grid(True, alpha=0.3)
    best = max(results, key=lambda r: r[4])
    axes[2].scatter([best[0]], [best[1]], marker='*', s=400, color='gold',
                    edgecolors='black', linewidths=1, zorder=5, label=f'Best {best[4]:.1f}')
    axes[2].legend(fontsize=9)

    plt.colorbar(sc2, ax=axes[2], label='Final Score (0–100)')
    plt.tight_layout()
    if save_files:
        fig.savefig(os.path.join(output_dir, 'final_score_sweep.png'), dpi=300, bbox_inches='tight')
        fig.savefig(os.path.join(output_dir, 'final_score_sweep.pdf'), bbox_inches='tight')
    if display_inline:
        plt.show()
    else:
        plt.close(fig)
    return fig


def plot_sweep_candidates(orig_acc=0.8, orig_vp1=60.0, v_limit=100.0,
                         display_inline=True, save_files=True, output_dir='fuzzy_graphs'):
    """
    Visualise the 5 × 4 = 20 design-parameter candidates swept in Pass 1.

    These are NOT fuzzy inputs — they are the parameters the optimizer tries.
    The fuzzy engines evaluate the simulation OUTPUTS (TravelTime, MotorPower)
    that each candidate produces, not the candidates themselves.

    Mirrors the sweep-building logic in optimization_handler.cpp:

        // acc_start_si: 5 values, 0.1 m/s² step, centred at originalAcc,
        //            clamped to [0.3, 1.5]
        for (int i = -2; i <= 2; ++i)  v = round(originalAcc + i*0.1, 1)

        // v_p1: 4 values at originalVp1 ± {15, 5, -5, -15} km/h,
        //       clamped to [20, v_limit - 5)
        for delta in {-15, -5, 5, 15}:  v = originalVp1 + delta

    Parameters
    ----------
    orig_acc  : float — user's loaded acc_start_si (m/s²), e.g. 0.8
    orig_vp1  : float — user's loaded v_p1 (km/h),    e.g. 60.0
    v_limit   : float — train's v_limit (km/h),        e.g. 100.0
    """
    # Build acc_start_si candidates (mirrors C++ loop)
    acc_candidates = []
    for i in range(-2, 3):
        v = round(orig_acc + i * 0.1, 1)
        if 0.3 <= v <= 1.5:
            acc_candidates.append(v)
    if not acc_candidates:
        acc_candidates = [orig_acc]

    # Build v_p1 candidates (mirrors C++ loop)
    vp1_candidates = []
    for delta in [-15, -5, 5, 15]:
        v = orig_vp1 + delta
        if 20.0 <= v < v_limit - 5.0:
            vp1_candidates.append(v)
    if not vp1_candidates:
        vp1_candidates = [orig_vp1]

    fig, axes = plt.subplots(1, 2, figsize=(14, 5))
    fig.suptitle(
        'Pass 1 Design-Parameter Sweep  (these are NOT fuzzy inputs)\n'
        'Each dot is one candidate that runs a full simulation → '
        'produces TravelTime + MotorPower → scored by fuzzy engines',
        fontsize=12, fontweight='bold')

    # ── Left: acc_start_si ─────────────────────────────────────────────────────
    ax = axes[0]
    colors_acc = ['steelblue' if v != orig_acc else 'gold' for v in acc_candidates]
    bars = ax.bar(range(len(acc_candidates)),
                  [1] * len(acc_candidates),
                  color=colors_acc, edgecolor='black', linewidth=1.2, width=0.5)

    for idx, v in enumerate(acc_candidates):
        label = f'{v:.1f}'
        if v == orig_acc:
            label += '\n(original)'
        ax.text(idx, 1.05, label, ha='center', va='bottom', fontsize=11,
                fontweight='bold' if v == orig_acc else 'normal')

    ax.set_xticks([])
    ax.set_yticks([])
    ax.set_ylim(0, 1.5)
    ax.set_xlim(-0.6, len(acc_candidates) - 0.4)
    ax.set_xlabel(f'Candidate index  (step = 0.1 m/s², clamped to [0.3, 1.5])',
                  fontsize=10, fontweight='bold')
    ax.set_title(f'acc_start_si candidates  ({len(acc_candidates)} values)\n'
                 f'Controls acceleration rate → affects TravelTime',
                 fontsize=11, fontweight='bold')

    # Add valid range bar at the bottom
    ax.axhline(0, color='gray', linewidth=0.5)
    ax.annotate('← valid range: 0.3 – 1.5 m/s² →',
                xy=(len(acc_candidates) / 2 - 0.5, -0.12),
                ha='center', fontsize=9, color='gray')

    gold_patch = mpatches.Patch(color='gold', label='Original value')
    blue_patch = mpatches.Patch(color='steelblue', label='Candidate (+/− 0.1 step)')
    ax.legend(handles=[gold_patch, blue_patch], fontsize=9, loc='upper right')

    # ── Right: v_p1 ─────────────────────────────────────────────────────────
    ax2 = axes[1]
    colors_vp1 = []
    labels_vp1 = []
    for v in vp1_candidates:
        delta = v - orig_vp1
        colors_vp1.append('coral' if delta < 0 else 'mediumseagreen')
        sign = '+' if delta >= 0 else ''
        labels_vp1.append(f'{v:.0f}\n({sign}{delta:.0f})')

    ax2.bar(range(len(vp1_candidates)),
            [1] * len(vp1_candidates),
            color=colors_vp1, edgecolor='black', linewidth=1.2, width=0.5)

    for idx, label in enumerate(labels_vp1):
        ax2.text(idx, 1.05, label, ha='center', va='bottom', fontsize=11)

    # Mark original with a dashed vertical line between candidates
    # (orig_vp1 is not itself a candidate — offsets are ±5 and ±15)
    orig_x = (len(vp1_candidates) - 1) / 2.0
    ax2.axvline(x=orig_x, color='gold', linestyle='--', linewidth=2,
                label=f'Original v_p1 = {orig_vp1:.0f} km/h (not a candidate)')

    ax2.set_xticks([])
    ax2.set_yticks([])
    ax2.set_ylim(0, 1.5)
    ax2.set_xlim(-0.6, len(vp1_candidates) - 0.4)
    ax2.set_xlabel(f'Candidate index  (offsets: −15, −5, +5, +15 km/h, '
                   f'clamped to [20, {v_limit - 5:.0f}] km/h)',
                   fontsize=10, fontweight='bold')
    ax2.set_title(f'v_p1 (weakening speed) candidates  ({len(vp1_candidates)} values)\n'
                  f'Controls field-weakening onset → affects MotorPower',
                  fontsize=11, fontweight='bold')

    coral_patch  = mpatches.Patch(color='coral',         label='Lower than original (less weakening)')
    green_patch  = mpatches.Patch(color='mediumseagreen', label='Higher than original (more weakening)')
    gold_line    = Line2D([0], [0], color='gold', linestyle='--', linewidth=2,
                          label=f'Original = {orig_vp1:.0f} km/h')
    ax2.legend(handles=[coral_patch, green_patch, gold_line], fontsize=9, loc='upper right')

    plt.tight_layout()
    if save_files:
        fig.savefig(os.path.join(output_dir, 'sweep_candidates.png'), dpi=300, bbox_inches='tight')
        fig.savefig(os.path.join(output_dir, 'sweep_candidates.pdf'), bbox_inches='tight')
    if display_inline:
        plt.show()
    else:
        plt.close(fig)
    return fig


def plot_acceleration_membership(acc_min=0.3, acc_linear=1.5,
                                 display_inline=True, save_files=True, output_dir='fuzzy_graphs'):
    """
    Plot hypothetical membership functions for acc_start_si (m/s²).

    acc_start_si is a design parameter, NOT a fuzzy input — the optimizer sweeps
    5 discrete values of it and runs simulations. This plot shows what the
    membership landscape would look like IF it were fuzzified, as a reference.

    Terms use the same dynamic-breakpoint formula as the real engines
    (5% margin + percentage fractions of the value range).
    """
    bp = compute_input_breakpoints(acc_min, acc_linear)
    lo, hi = bp['min'], bp['max']
    pad = (hi - lo) * 0.05
    x = np.linspace(lo - pad, hi + pad, 2000)

    label_map = {'Short_Low': 'Low (slow accel)', 'Medium': 'Medium', 'Long_High': 'High (fast accel)'}

    fig, ax = plt.subplots(figsize=(10, 5))
    for key, color in INPUT_COLORS.items():
        mf = eval_term(bp[key], x)
        ax.fill_between(x, 0, mf, alpha=0.20, color=color)
        ax.plot(x, mf, color=color, linewidth=2.5, label=label_map[key])

    ax.set_xlabel('Acceleration acc_start_si (m/s²)', fontsize=11, fontweight='bold')
    ax.set_ylabel('Membership Degree', fontsize=11, fontweight='bold')
    ax.set_title(
        'acc_start_si — Hypothetical Membership Functions\n'
        '(design parameter, not a fuzzy engine input — shown for reference only)',
        fontsize=12, fontweight='bold')
    ax.legend(fontsize=10)
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.set_xlim(lo - pad, hi + pad)
    ax.set_ylim(-0.05, 1.15)
    ax.annotate(
        f'Actual sweep: 5 discrete values centred at user acc_start_si,\nstep = 0.1 m/s², clamped to [{acc_min}, {acc_linear}] m/s²',
        xy=(0.02, 0.05), xycoords='axes fraction', fontsize=9,
        bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.7))

    plt.tight_layout()
    if save_files:
        fig.savefig(os.path.join(output_dir, 'acceleration_mf.png'), dpi=300, bbox_inches='tight')
        fig.savefig(os.path.join(output_dir, 'acceleration_mf.pdf'), bbox_inches='tight')
    if display_inline:
        plt.show()
    else:
        plt.close(fig)
    return fig


def plot_weakening_speed_membership(vp1_center=60.0, v_limit=100.0,
                                    display_inline=True, save_files=True, output_dir='fuzzy_graphs'):
    """
    Plot hypothetical membership functions for v_p1 (field-weakening speed, km/h).

    v_p1 is a design parameter, NOT a fuzzy input — the optimizer sweeps
    4 discrete offsets (±5, ±15 km/h) around the user's value. This plot
    shows what the membership landscape would look like IF it were fuzzified.

    Terms use the same dynamic-breakpoint formula as the real engines
    (5% margin + percentage fractions of the value range), applied over the
    valid candidate range [20, v_limit − 5) km/h.
    """
    vp1_min = 20.0
    vp1_max = v_limit - 5.0
    bp = compute_input_breakpoints(vp1_min, vp1_max)
    lo, hi = bp['min'], bp['max']
    pad = (hi - lo) * 0.05
    x = np.linspace(lo - pad, hi + pad, 2000)

    label_map = {
        'Short_Low': 'Low (early weakening)',
        'Medium':    'Medium',
        'Long_High': 'High (late weakening)',
    }

    fig, ax = plt.subplots(figsize=(10, 5))
    for key, color in INPUT_COLORS.items():
        mf = eval_term(bp[key], x)
        ax.fill_between(x, 0, mf, alpha=0.20, color=color)
        ax.plot(x, mf, color=color, linewidth=2.5, label=label_map[key])

    # Mark the 4 candidate offsets around vp1_center
    for delta, ls in [(-15, ':'), (-5, '--'), (5, '--'), (15, ':')]:
        v = vp1_center + delta
        if vp1_min <= v < vp1_max:
            ax.axvline(x=v, color='navy', linestyle=ls, linewidth=1.4,
                       label=f'candidate {v:.0f} km/h ({delta:+d})')
    ax.axvline(x=vp1_center, color='gold', linestyle='-', linewidth=2,
               label=f'original v_p1 = {vp1_center:.0f} km/h')

    ax.set_xlabel('Weakening Speed v_p1 (km/h)', fontsize=11, fontweight='bold')
    ax.set_ylabel('Membership Degree', fontsize=11, fontweight='bold')
    ax.set_title(
        'v_p1 (field-weakening speed) — Hypothetical Membership Functions\n'
        '(design parameter, not a fuzzy engine input — shown for reference only)',
        fontsize=12, fontweight='bold')
    ax.legend(fontsize=9, loc='upper right')
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.set_xlim(lo - pad, hi + pad)
    ax.set_ylim(-0.05, 1.15)
    ax.annotate(
        f'Actual sweep: 4 offsets (−15, −5, +5, +15 km/h) from original,\nclamped to [20, {vp1_max:.0f}] km/h',
        xy=(0.02, 0.05), xycoords='axes fraction', fontsize=9,
        bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.7))

    plt.tight_layout()
    if save_files:
        fig.savefig(os.path.join(output_dir, 'weakening_speed_mf.png'), dpi=300, bbox_inches='tight')
        fig.savefig(os.path.join(output_dir, 'weakening_speed_mf.pdf'), bbox_inches='tight')
    if display_inline:
        plt.show()
    else:
        plt.close(fig)
    return fig


def plot_overview_grid(raw_min_t, raw_max_t, raw_min_p, raw_max_p,
                       display_inline=True, save_files=True, output_dir='fuzzy_graphs'):
    """
    One-page 2×3 overview grid:
      [0,0] TravelTime input MFs     [0,1] TimeScore output MFs
      [1,0] MotorPower input MFs     [1,1] PowerScore output MFs
      [0,2] Rule table text          [1,2] Architecture overview text
    """
    bp_t = compute_input_breakpoints(raw_min_t, raw_max_t)
    bp_p = compute_input_breakpoints(raw_min_p, raw_max_p)

    fig, axes = plt.subplots(2, 3, figsize=(18, 10))
    fig.suptitle('Fuzzy Optimization System — Complete Overview\n'
                 'Two Mamdani engines, Mamdani implication, MAX aggregation, COG defuzzification',
                 fontsize=13, fontweight='bold')

    _plot_input_mf(axes[0, 0], bp_t, INPUT_LABEL_MAP,
                   'Travel Time (s)', '(a) TravelTime Input')
    _plot_output_mf(axes[0, 1], '(b) TimeScore Output (0–100)')
    _plot_input_mf(axes[1, 0], bp_p, POWER_LABEL_MAP,
                   'Motor Power (kW)', '(c) MotorPower Input')
    _plot_output_mf(axes[1, 1], '(d) PowerScore Output (0–100)')

    # Rule table
    rule_text = (
        "RULES — m_timeEngine\n"
        "─────────────────────────────\n"
        "IF TravelTime = Short   → Excellent\n"
        "IF TravelTime = Medium  → Good\n"
        "IF TravelTime = Long    → Poor\n\n"
        "RULES — m_powerEngine\n"
        "─────────────────────────────\n"
        "IF MotorPower = Low     → Excellent\n"
        "IF MotorPower = Medium  → Good\n"
        "IF MotorPower = High    → Poor\n"
    )
    axes[0, 2].text(0.05, 0.95, rule_text, transform=axes[0, 2].transAxes,
                    fontsize=10, va='top', fontfamily='monospace',
                    bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.8))
    axes[0, 2].axis('off')
    axes[0, 2].set_title('(e) Rule Base', fontsize=12, fontweight='bold')

    # Architecture overview
    arch_text = (
        "ARCHITECTURE\n"
        "═══════════════════════\n\n"
        "Input  → TravelTime (s)\n"
        "         MotorPower (kW)\n\n"
        "Engine → Mamdani type\n"
        "  • MIN  implication\n"
        "  • MAX  aggregation\n"
        "  • COG  defuzzification\n"
        "       (101 sample pts)\n\n"
        "Output → TimeScore  [0,100]\n"
        "         PowerScore [0,100]\n\n"
        "Final Score\n"
        "  = (TimeScore + PowerScore)\n"
        "    ─────────────────────────\n"
        "              2\n\n"
        "Ranges calibrated from\n"
        "actual Pass 1 results."
    )
    axes[1, 2].text(0.05, 0.95, arch_text, transform=axes[1, 2].transAxes,
                    fontsize=10, va='top', fontfamily='monospace',
                    bbox=dict(boxstyle='round', facecolor='lightcyan', alpha=0.8))
    axes[1, 2].axis('off')
    axes[1, 2].set_title('(f) System Architecture', fontsize=12, fontweight='bold')

    plt.tight_layout()
    if save_files:
        fig.savefig(os.path.join(output_dir, 'overview_grid.png'), dpi=300, bbox_inches='tight')
        fig.savefig(os.path.join(output_dir, 'overview_grid.pdf'), bbox_inches='tight')
    if display_inline:
        plt.show()
    else:
        plt.close(fig)
    return fig





# =============================================================================
# ── SECTION 5: main() ────────────────────────────────────────────────────────
# =============================================================================

def main(display_inline=True, save_files=True,
         min_time=350.0, max_time=750.0,
         min_power=150.0, max_power=500.0,
         example_time=None, example_power=None,
         orig_acc=0.8, orig_vp1=60.0, v_limit=100.0):
    """
    Generate all fuzzy membership plots for both engines.

    Parameters
    ----------
    display_inline  : bool   — show plots inline (True in Colab/Jupyter)
    save_files      : bool   — write PNG + PDF to output_dir
    min_time        : float  — raw minimum TravelTime from Pass 1 (seconds)
    max_time        : float  — raw maximum TravelTime from Pass 1 (seconds)
    min_power       : float  — raw minimum MotorPower from Pass 1 (kW)
    max_power       : float  — raw maximum MotorPower from Pass 1 (kW)
    example_time    : float  — optional single crisp TravelTime to demonstrate
                               fuzzification + COG (default: midpoint of range)
    example_power   : float  — optional single crisp MotorPower to demonstrate
                               fuzzification + COG (default: midpoint of range)
    orig_acc        : float  — user's original acc_start_si value (m/s²), used to
                               build the 5-candidate sweep plot (default: 0.8)
    orig_vp1        : float  — user's original v_p1 (km/h), used to build the
                               4-candidate sweep plot (default: 60.0)
    v_limit         : float  — train's speed limit (km/h), caps v_p1 candidates
                               (default: 100.0)

    Usage in Colab
    --------------
    main()                              # all defaults
    main(min_time=300, max_time=800,    # real Pass 1 data
         min_power=120, max_power=450,
         example_time=520,              # show fuzzification for 520 s
         example_power=280,             # show fuzzification for 280 kW
         orig_acc=0.9, orig_vp1=55.0,  # your actual train parameters
         v_limit=90.0)
    """
    output_dir = 'fuzzy_graphs'
    if save_files:
        os.makedirs(output_dir, exist_ok=True)

    # Default example values = midpoint of user range
    if example_time is None:
        example_time = (min_time + max_time) / 2.0
    if example_power is None:
        example_power = (min_power + max_power) / 2.0

    sep = '=' * 62

    print(sep)
    print(f"  Fuzzy Membership Visualizer — Train Optimization")
    print(f"  TravelTime  range : {min_time:.0f} – {max_time:.0f} s")
    print(f"  MotorPower  range : {min_power:.0f} – {max_power:.0f} kW")
    print(f"  Example TravelTime  : {example_time:.1f} s")
    print(f"  Example MotorPower  : {example_power:.1f} kW")
    print(sep)

    # ── 1. Sweep candidates (acc_start_si + v_p1 — design parameters) ──────────
    print(f"\n[1/8] Pass 1 sweep candidates — acc_start_si ({orig_acc} m/s²) + v_p1 ({orig_vp1} km/h)")
    plot_sweep_candidates(orig_acc=orig_acc, orig_vp1=orig_vp1, v_limit=v_limit,
                          display_inline=display_inline,
                          save_files=save_files,
                          output_dir=output_dir)

    # ── 2. Acceleration membership (reference plot) ──────────────────────────
    print(f"\n[2/8] acc_start_si — hypothetical membership functions (reference)")
    plot_acceleration_membership(acc_min=0.3, acc_linear=1.5,
                                 display_inline=display_inline,
                                 save_files=save_files,
                                 output_dir=output_dir)

    # ── 3. Weakening speed membership (reference plot) ───────────────────────
    print(f"\n[3/8] v_p1 — hypothetical membership functions (reference)")
    plot_weakening_speed_membership(vp1_center=orig_vp1, v_limit=v_limit,
                                    display_inline=display_inline,
                                    save_files=save_files,
                                    output_dir=output_dir)

    # ── 4. Time Engine ──────────────────────────────────────────────────────
    print(f"\n[4/8] m_timeEngine — TravelTime → TimeScore")
    plot_time_engine(min_time, max_time,
                     example_t=example_time,
                     display_inline=display_inline,
                     save_files=save_files,
                     output_dir=output_dir)

    # ── 5. Power Engine ─────────────────────────────────────────────────────
    print(f"\n[5/8] m_powerEngine — MotorPower → PowerScore")
    plot_power_engine(min_power, max_power,
                      example_p=example_power,
                      display_inline=display_inline,
                      save_files=save_files,
                      output_dir=output_dir)

    # ── 6. Shared output score MFs ──────────────────────────────────────────
    print(f"\n[6/8] Output Score MFs (shared: Poor / Fair / Good / Excellent)")
    plot_output_score_mf(display_inline=display_inline,
                         save_files=save_files,
                         output_dir=output_dir)

    # ── 7. Final score sweep ─────────────────────────────────────────────────
    print(f"\n[7/8] Final Score sweep (4×5 demo grid)")
    plot_final_score_sweep(min_time, max_time, min_power, max_power,
                           display_inline=display_inline,
                           save_files=save_files,
                           output_dir=output_dir)

    # ── 8. Overview grid ─────────────────────────────────────────────────────
    print(f"\n[8/8] Overview grid (all components on one page)")
    plot_overview_grid(min_time, max_time, min_power, max_power,
                       display_inline=display_inline,
                       save_files=save_files,
                       output_dir=output_dir)

    # ── Final score for the example values ──────────────────────────────────
    bp_t = compute_input_breakpoints(min_time, max_time)
    bp_p = compute_input_breakpoints(min_power, max_power)
    rules = [('Short_Low', 'Excellent'), ('Medium', 'Good'), ('Long_High', 'Poor')]
    ts, _, _, _ = cog_defuzzify(example_time,  bp_t, [], rules)
    ps, _, _, _ = cog_defuzzify(example_power, bp_p, [], rules)
    final = (ts + ps) / 2.0

    print(f"\n{sep}")
    print(f"  Example computation summary")
    print(f"  TravelTime  = {example_time:.1f} s  →  TimeScore  = {ts:.2f}")
    print(f"  MotorPower  = {example_power:.1f} kW  →  PowerScore = {ps:.2f}")
    print(f"  Final Score = ({ts:.2f} + {ps:.2f}) / 2 = {final:.2f} / 100")
    if save_files:
        print(f"\n  Files saved to: {output_dir}/")
        print(f"  sweep_candidates.png/pdf")
        print(f"  acceleration_mf.png/pdf")
        print(f"  weakening_speed_mf.png/pdf")
        print(f"  time_engine.png/pdf")
        print(f"  power_engine.png/pdf")
        print(f"  output_score_mf.png/pdf")
        print(f"  final_score_sweep.png/pdf")
        print(f"  overview_grid.png/pdf")
        print(f"\n  In Colab: Files panel (left) → {output_dir}/ → right-click → Download")
    print(sep)


if __name__ == "__main__":
    # When run locally, save files but don't display inline
    main(display_inline=False, save_files=True)
