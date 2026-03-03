#include "optimization_handler.h"
#include "controllers/simulation/train_simulation_handler.h"
#include "utils/fuzzy_engine.h"
#include <QDebug>
#include <algorithm>
#include <memory>

OptimizationHandler::OptimizationHandler(
    AppContext *context, TrainSimulationHandler *simulationHandler,
    QObject *parent)
    : QObject(parent), m_simulationDatas(context->simulationDatas.data()),
      m_movingData(context->movingData.data()),
      m_trainSimulation(simulationHandler),
      m_simulationMutex(&context->simulationMutex) {
  // Fuzzy engine is calibrated from actual sweep data — no static setup here.
}

// =============================================================================
// Fuzzy Engine Setup — called AFTER Pass 1 so ranges come from real data.
//   minT/maxT : actual min/max travel time across all sweep results (s)
//   minP/maxP : actual min/max peak motor power across all sweep results (kW)
// =============================================================================
void OptimizationHandler::setupFuzzyEngine(double minT, double maxT,
                                           double minP, double maxP) {
  m_fuzzyEngine.clear();

  // 5% margin so boundary values still get non-zero membership
  const double mT = (maxT - minT) * 0.05;
  const double mP = (maxP - minP) * 0.05;
  minT -= mT;
  maxT += mT;
  minP -= mP;
  maxP += mP;

  const double rT = maxT - minT;
  const double rP = maxP - minP;

  // ── Input 1: Travel Time — Short=fast(good), Long=slow(bad) ─────────────
  auto travelTime = std::make_shared<FuzzyVariable>("TravelTime", minT, maxT);
  travelTime->addTerm(std::make_shared<TrapezoidSet>(
      "Short", minT, minT, minT + 0.25 * rT, minT + 0.45 * rT));
  travelTime->addTerm(std::make_shared<TriangleSet>(
      "Medium", minT + 0.30 * rT, minT + 0.50 * rT, minT + 0.70 * rT));
  travelTime->addTerm(std::make_shared<TrapezoidSet>(
      "Long", minT + 0.55 * rT, minT + 0.75 * rT, maxT, maxT));

  // ── Input 2: Motor Power — Low=efficient(good), High=hungry(bad) ─────────
  auto motorPower = std::make_shared<FuzzyVariable>("MotorPower", minP, maxP);
  motorPower->addTerm(std::make_shared<TrapezoidSet>(
      "Low", minP, minP, minP + 0.25 * rP, minP + 0.45 * rP));
  motorPower->addTerm(std::make_shared<TriangleSet>(
      "Medium", minP + 0.30 * rP, minP + 0.50 * rP, minP + 0.70 * rP));
  motorPower->addTerm(std::make_shared<TrapezoidSet>(
      "High", minP + 0.55 * rP, minP + 0.75 * rP, maxP, maxP));

  // ── Output: Fuzzy Score (0–100) ──────────────────────────────────────────
  auto fuzzyScore = std::make_shared<FuzzyVariable>("FuzzyScore", 0.0, 100.0);
  fuzzyScore->addTerm(
      std::make_shared<TrapezoidSet>("Poor", 0.0, 0.0, 15.0, 30.0));
  fuzzyScore->addTerm(std::make_shared<TriangleSet>("Fair", 20.0, 38.0, 55.0));
  fuzzyScore->addTerm(std::make_shared<TriangleSet>("Good", 45.0, 62.0, 78.0));
  fuzzyScore->addTerm(
      std::make_shared<TrapezoidSet>("Excellent", 68.0, 82.0, 100.0, 100.0));

  m_fuzzyEngine.addInputVariable(travelTime);
  m_fuzzyEngine.addInputVariable(motorPower);
  m_fuzzyEngine.addOutputVariable(fuzzyScore);

  // ── 9 Mamdani Rules (3 time × 3 power) ──────────────────────────────────
  auto makeRule = [](const QString &time, const QString &power,
                     const QString &score) -> FuzzyRule {
    FuzzyRule r;
    r.antecedents["TravelTime"] = time;
    r.antecedents["MotorPower"] = power;
    r.consequent = {"FuzzyScore", score};
    return r;
  };
  m_fuzzyEngine.addRule(makeRule("Short", "Low", "Excellent"));
  m_fuzzyEngine.addRule(makeRule("Short", "Medium", "Good"));
  m_fuzzyEngine.addRule(makeRule("Short", "High", "Fair"));
  m_fuzzyEngine.addRule(makeRule("Medium", "Low", "Good"));
  m_fuzzyEngine.addRule(makeRule("Medium", "Medium", "Good"));
  m_fuzzyEngine.addRule(makeRule("Medium", "High", "Fair"));
  m_fuzzyEngine.addRule(makeRule("Long", "Low", "Fair"));
  m_fuzzyEngine.addRule(makeRule("Long", "Medium", "Poor"));
  m_fuzzyEngine.addRule(makeRule("Long", "High", "Poor"));

  qDebug() << "Fuzzy engine calibrated — TravelTime [" << minT << "–" << maxT
           << "] s  |  MotorPower [" << minP << "–" << maxP << "] kW";
}

double OptimizationHandler::evaluateFuzzyScore(double travelTime,
                                               double motorPower) {
  m_fuzzyEngine.setInputValue("TravelTime", travelTime);
  m_fuzzyEngine.setInputValue("MotorPower", motorPower);
  return m_fuzzyEngine.getOutputValue("FuzzyScore");
}

// =============================================================================
// Main Optimization Loop — TWO PASS
//   Pass 1: run all 20 simulations, collect (travelTime, peakPower) per combo
//   Pass 2: calibrate fuzzy engine from actual observed ranges, then score
// This makes the engine work with ANY train + track configuration.
// =============================================================================
void OptimizationHandler::handleOptimization() {
  if (m_isRunning.testAndSetRelaxed(0, 1) == false) {
    qWarning() << "Optimization already running, ignoring request.";
    return;
  }

  // Require at least one successful simulation to have valid parameters
  if (m_trainSimulation->getMaxSpeed() <= 0.0) {
    qWarning() << "Optimization aborted: run a dynamic simulation first.";
    m_isRunning.storeRelaxed(0);
    return;
  }

  // Save and restore user's original parameters
  const double originalAcc = m_movingData->acc_start;
  const double originalVp1 = m_movingData->v_p1;

  // Build sweep centered on the user's ACTUAL loaded parameters.
  // This makes the optimizer adapt to any train configuration.
  //   acc  : 5 values, 0.1 m/s² step, centred at originalAcc, clamped
  //   [0.3, 1.5] v_p1 : 4 values at -15, -5, +5, +15 km/h from originalVp1,
  //          clamped to [20, v_limit - 5)
  QList<double> accValues;
  for (int i = -2; i <= 2; ++i) {
    const double v =
        std::round((originalAcc + i * 0.1) * 10.0) / 10.0; // avoid FP drift
    if (v >= 0.3 && v <= 1.5)
      accValues.append(v);
  }
  if (accValues.isEmpty())
    accValues.append(originalAcc);

  const double vLimitKmh = m_movingData->v_limit;
  QList<double> vp1Values;
  for (int delta : {-15, -5, 5, 15}) {
    const double v = originalVp1 + delta;
    if (v >= 20.0 && v < vLimitKmh - 5.0)
      vp1Values.append(v);
  }
  if (vp1Values.isEmpty())
    vp1Values.append(originalVp1);

  qDebug() << "Optimization sweep — acc:" << accValues
           << "| v_p1:" << vp1Values;

  m_results.clear();

  // ── PASS 1: simulate all combinations, record raw metrics ──────────────
  qDebug() << "=== Optimization Pass 1: running"
           << accValues.size() * vp1Values.size() << "simulations ===";

  struct RawEntry {
    double acc, vp1, peakPower, travelTime;
  };
  QList<RawEntry> rawData;

  for (double acc : accValues) {
    for (double vp1 : vp1Values) {
      m_movingData->acc_start = acc;
      m_movingData->v_p1 = vp1;
      m_trainSimulation->runDynamicSimulation();

      if (m_simulationDatas->powerMotorOutPerMotor.isEmpty() ||
          m_simulationDatas->timeTotal.isEmpty()) {
        qWarning() << "Pass1: empty result for acc=" << acc << "vp1=" << vp1;
        continue;
      }

      RawEntry e;
      e.acc = acc;
      e.vp1 = vp1;
      e.peakPower = findMaximumPowerMotorPerCar();
      e.travelTime = m_simulationDatas->timeTotal.last();
      rawData.append(e);

      qDebug()
          << QString(
                 "  acc=%-4.2f  vp1=%-5.1f  peakPwr=%-8.1f kW  time=%-8.1f s")
                 .arg(acc)
                 .arg(vp1)
                 .arg(e.peakPower)
                 .arg(e.travelTime);
    }
  }

  // Always restore user parameters, even if Pass 1 produced zero results
  m_movingData->acc_start = originalAcc;
  m_movingData->v_p1 = originalVp1;

  if (rawData.isEmpty()) {
    qWarning() << "Optimization: no valid results from Pass 1.";
    m_isRunning.storeRelaxed(0);
    return;
  }

  // ── PASS 2: derive ranges from actual data, calibrate engine, score ─────
  double minT = rawData[0].travelTime, maxT = rawData[0].travelTime;
  double minP = rawData[0].peakPower, maxP = rawData[0].peakPower;
  for (const RawEntry &e : rawData) {
    minT = std::min(minT, e.travelTime);
    maxT = std::max(maxT, e.travelTime);
    minP = std::min(minP, e.peakPower);
    maxP = std::max(maxP, e.peakPower);
  }
  // Avoid degenerate (all-same) range
  if (maxT - minT < 1.0) {
    minT -= 1.0;
    maxT += 1.0;
  }
  if (maxP - minP < 1.0) {
    minP -= 1.0;
    maxP += 1.0;
  }

  setupFuzzyEngine(minT, maxT, minP, maxP);

  qDebug() << "=== Optimization Pass 2: scoring ===";
  for (const RawEntry &e : rawData) {
    OptResult r;
    r.acc_start = e.acc;
    r.v_p1 = e.vp1;
    r.peakMotorPower = e.peakPower;
    r.travelTime = e.travelTime;
    r.fuzzyScore = evaluateFuzzyScore(e.travelTime, e.peakPower);
    m_results.append(r);

    qDebug() << QString("  acc=%-4.2f  vp1=%-5.1f  score=%-6.2f")
                    .arg(e.acc)
                    .arg(e.vp1)
                    .arg(r.fuzzyScore);
  }

  m_bestResult = *std::max_element(m_results.begin(), m_results.end(),
                                   [](const OptResult &a, const OptResult &b) {
                                     return a.fuzzyScore < b.fuzzyScore;
                                   });

  qDebug() << "=== Optimization Complete ==="
           << "| WINNER: acc=" << m_bestResult.acc_start
           << "vp1=" << m_bestResult.v_p1
           << "score=" << m_bestResult.fuzzyScore;

  emit optimizationComplete(m_bestResult);
  m_isRunning.storeRelaxed(0);
}

double OptimizationHandler::findMaximumPowerMotorPerCar() {
  if (m_simulationDatas->powerMotorOutPerMotor.isEmpty())
    return 0.0;
  return *std::max_element(m_simulationDatas->powerMotorOutPerMotor.begin(),
                           m_simulationDatas->powerMotorOutPerMotor.end());
}
