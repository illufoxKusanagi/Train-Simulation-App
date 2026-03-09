#include "optimization_handler.h"
#include "controllers/optimization/fuzzy/trapezoid_set.h"
#include "controllers/optimization/fuzzy/triangle_set.h"
#include "controllers/simulation/train_simulation_handler.h"
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
// Time Engine Setup — evaluates TravelTime (shaped by acc_start).
//
// A shorter travel time means acc_start enabled faster acceleration → better.
// 3 input terms (Short/Medium/Long) × 1 output (TimeScore 0–100) = 3 rules.
// Ranges are derived from actual Pass 1 data.
// =============================================================================
void OptimizationHandler::setupTimeEngine(double minT, double maxT) {
  m_timeEngine.clear();

  // 5% margin so boundary results still get non-zero membership
  const double margin = (maxT - minT) * 0.05;
  minT -= margin;
  maxT += margin;
  const double r = maxT - minT;

  // ── Input: TravelTime — Short=fast(good), Long=slow(bad) ─────────────────
  auto travelTime = std::make_shared<FuzzyVariable>("TravelTime", minT, maxT);
  travelTime->addTerm(std::make_shared<TrapezoidSet>(
      "Short", minT, minT, minT + 0.25 * r, minT + 0.45 * r));
  travelTime->addTerm(std::make_shared<TriangleSet>(
      "Medium", minT + 0.30 * r, minT + 0.50 * r, minT + 0.70 * r));
  travelTime->addTerm(std::make_shared<TrapezoidSet>(
      "Long", minT + 0.55 * r, minT + 0.75 * r, maxT, maxT));

  // ── Output: TimeScore (0–100) ─────────────────────────────────────────────
  auto timeScore = std::make_shared<FuzzyVariable>("TimeScore", 0.0, 100.0);
  timeScore->addTerm(
      std::make_shared<TrapezoidSet>("Poor", 0.0, 0.0, 15.0, 30.0));
  timeScore->addTerm(std::make_shared<TriangleSet>("Fair", 20.0, 38.0, 55.0));
  timeScore->addTerm(std::make_shared<TriangleSet>("Good", 45.0, 62.0, 78.0));
  timeScore->addTerm(
      std::make_shared<TrapezoidSet>("Excellent", 68.0, 82.0, 100.0, 100.0));

  m_timeEngine.addInputVariable(travelTime);
  m_timeEngine.addOutputVariable(timeScore);

  // ── 3 Rules for TravelTime ────────────────────────────────────────────────
  // Rule: IF TravelTime is X → TimeScore is Y
  auto makeRule = [](const QString &term, const QString &score) -> FuzzyRule {
    FuzzyRule r;
    r.antecedents["TravelTime"] = term;
    r.consequent = {"TimeScore", score};
    return r;
  };
  m_timeEngine.addRule(makeRule("Short", "Excellent"));
  m_timeEngine.addRule(makeRule("Medium", "Good"));
  m_timeEngine.addRule(makeRule("Long", "Poor"));

  qDebug() << "Time engine calibrated — TravelTime [" << minT << "–" << maxT
           << "] s";
}

// =============================================================================
// Power Engine Setup — evaluates MotorPower (shaped by v_p1).
//
// A lower peak power means v_p1 allowed the motor to work more efficiently.
// 3 input terms (Low/Medium/High) × 1 output (PowerScore 0–100) = 3 rules.
// Ranges are derived from actual Pass 1 data.
// =============================================================================
void OptimizationHandler::setupPowerEngine(double minP, double maxP) {
  m_powerEngine.clear();

  // 5% margin so boundary results still get non-zero membership
  const double margin = (maxP - minP) * 0.05;
  minP -= margin;
  maxP += margin;
  const double r = maxP - minP;

  // ── Input: MotorPower — Low=efficient(good), High=hungry(bad) ────────────
  auto motorPower = std::make_shared<FuzzyVariable>("MotorPower", minP, maxP);
  motorPower->addTerm(std::make_shared<TrapezoidSet>(
      "Low", minP, minP, minP + 0.25 * r, minP + 0.45 * r));
  motorPower->addTerm(std::make_shared<TriangleSet>(
      "Medium", minP + 0.30 * r, minP + 0.50 * r, minP + 0.70 * r));
  motorPower->addTerm(std::make_shared<TrapezoidSet>(
      "High", minP + 0.55 * r, minP + 0.75 * r, maxP, maxP));

  // ── Output: PowerScore (0–100) ────────────────────────────────────────────
  auto powerScore = std::make_shared<FuzzyVariable>("PowerScore", 0.0, 100.0);
  powerScore->addTerm(
      std::make_shared<TrapezoidSet>("Poor", 0.0, 0.0, 15.0, 30.0));
  powerScore->addTerm(std::make_shared<TriangleSet>("Fair", 20.0, 38.0, 55.0));
  powerScore->addTerm(std::make_shared<TriangleSet>("Good", 45.0, 62.0, 78.0));
  powerScore->addTerm(
      std::make_shared<TrapezoidSet>("Excellent", 68.0, 82.0, 100.0, 100.0));

  m_powerEngine.addInputVariable(motorPower);
  m_powerEngine.addOutputVariable(powerScore);

  // ── 3 Rules for MotorPower ────────────────────────────────────────────────
  // Rule: IF MotorPower is X → PowerScore is Y
  auto makeRule = [](const QString &term, const QString &score) -> FuzzyRule {
    FuzzyRule r;
    r.antecedents["MotorPower"] = term;
    r.consequent = {"PowerScore", score};
    return r;
  };
  m_powerEngine.addRule(makeRule("Low", "Excellent"));
  m_powerEngine.addRule(makeRule("Medium", "Good"));
  m_powerEngine.addRule(makeRule("High", "Poor"));

  qDebug() << "Power engine calibrated — MotorPower [" << minP << "–" << maxP
           << "] kW";
}

// Final score = average of both sub-scores.
// This gives equal weight to travel time quality (acc_start contribution)
// and motor power quality (v_p1 contribution).
double OptimizationHandler::evaluateFuzzyScore(double travelTime,
                                               double motorPower) {
  m_timeEngine.setInputValue("TravelTime", travelTime);
  m_powerEngine.setInputValue("MotorPower", motorPower);
  const double timeScore = m_timeEngine.getOutputValue("TimeScore");
  const double powerScore = m_powerEngine.getOutputValue("PowerScore");
  return (timeScore + powerScore) / 2.0;
}

// =============================================================================
// Main Optimization Loop — TWO PASS
//   Pass 1: run all 20 simulations, collect (travelTime, peakPower) per combo
//   Pass 2: calibrate fuzzy engine from actual observed ranges, then score
// This makes the engine work with ANY train + track configuration.
// =============================================================================
void OptimizationHandler::handleOptimization(
    const QList<double> &accCandidates, const QList<double> &vp1Candidates) {
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

  // Use the caller-supplied candidates directly.
  // Fallback to the user's current parameters if the lists are empty.
  QList<double> accValues =
      accCandidates.isEmpty() ? QList<double>{originalAcc} : accCandidates;
  QList<double> vp1Values =
      vp1Candidates.isEmpty() ? QList<double>{originalVp1} : vp1Candidates;

  m_totalCombinations.storeRelaxed(accValues.size() * vp1Values.size());

  qDebug() << "Optimization sweep — acc:" << accValues
           << "| v_p1:" << vp1Values;

  {
    QMutexLocker lk(&m_resultsMutex);
    m_results.clear();
  }

  // ── PASS 1: simulate all combinations, record raw metrics ──────────────
  qDebug() << "=== Optimization Pass 1: running"
           << accValues.size() * vp1Values.size() << "simulations ===";

  struct RawEntry {
    double acc, vp1, peakPower, travelTime;
  };
  QList<RawEntry> rawData;

  for (double acc : accValues) {
    for (double vp1 : vp1Values) {
      // Set parameters and run simulation WITHOUT holding the outer mutex.
      // runDynamicSimulation() acquires m_simulationMutex internally for each
      // step — holding it here too would deadlock (QMutex is non-recursive).
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

  // Restore user parameters (no mutex needed — simulation is done and we own
  // m_movingData exclusively while m_isRunning == 1).
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

  setupTimeEngine(minT, maxT);
  setupPowerEngine(minP, maxP);

  qDebug() << "=== Optimization Pass 2: scoring ===";
  {
    QMutexLocker lk(&m_resultsMutex);
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

    m_bestResult =
        *std::max_element(m_results.begin(), m_results.end(),
                          [](const OptResult &a, const OptResult &b) {
                            return a.fuzzyScore < b.fuzzyScore;
                          });
  } // m_resultsMutex released

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
