#include "optimization_handler.h"
#include "controllers/optimization/fuzzy/trapezoid_set.h"
#include "controllers/optimization/fuzzy/triangle_set.h"
#include "controllers/simulation/train_simulation_handler.h"
#include <algorithm>
#include <memory>

OptimizationHandler::OptimizationHandler(
    AppContext *context, TrainSimulationHandler *simulationHandler,
    QObject *parent)
    : QObject(parent), m_simulationDatas(context->simulationDatas.data()),
      m_movingData(context->movingData.data()),
      m_trainSimulation(simulationHandler),
      m_simulationMutex(&context->simulationMutex) {}

// =============================================================================
// Time Engine Setup — evaluates TravelTime.
//
// better. 3 input terms (Short/Medium/Long) × 1 output (TimeScore 0–100) = 3
// rules. Ranges are derived from actual Pass 1 data.
// =============================================================================
void OptimizationHandler::setupTimeEngine(double minT, double maxT) {
  m_timeEngine.clear();

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
}

// =============================================================================
// Power Engine Setup — evaluates MotorPower.
//
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
}

// =============================================================================
// Energy Engine Setup — evaluates EnergyConsumption
//
// 3 input terms (Low/Medium/High) × 1 output (EnergyScore 0–100) = 3 rules.
// Ranges are derived from actual Pass 1 data.
// =============================================================================
void OptimizationHandler::setupEnergyEngine(double minE, double maxE) {
  m_energyEngine.clear();

  // 5% margin so boundary results still get non-zero membership
  const double margin = (maxE - minE) * 0.05;
  minE -= margin;
  maxE += margin;
  const double r = maxE - minE;

  // ── Input: EnergyConsumption — Low=efficient(good), High=hungry(bad)
  // ────────────
  auto energyConsumption =
      std::make_shared<FuzzyVariable>("EnergyConsumption", minE, maxE);
  energyConsumption->addTerm(std::make_shared<TrapezoidSet>(
      "Low", minE, minE, minE + 0.25 * r, minE + 0.45 * r));
  energyConsumption->addTerm(std::make_shared<TriangleSet>(
      "Medium", minE + 0.30 * r, minE + 0.50 * r, minE + 0.70 * r));
  energyConsumption->addTerm(std::make_shared<TrapezoidSet>(
      "High", minE + 0.55 * r, minE + 0.75 * r, maxE, maxE));

  // ── Output: EnergyScore (0–100) ────────────────────────────────────────────
  auto energyScore = std::make_shared<FuzzyVariable>("EnergyScore", 0.0, 100.0);
  energyScore->addTerm(
      std::make_shared<TrapezoidSet>("Poor", 0.0, 0.0, 15.0, 30.0));
  energyScore->addTerm(std::make_shared<TriangleSet>("Fair", 20.0, 38.0, 55.0));
  energyScore->addTerm(std::make_shared<TriangleSet>("Good", 45.0, 62.0, 78.0));
  energyScore->addTerm(
      std::make_shared<TrapezoidSet>("Excellent", 68.0, 82.0, 100.0, 100.0));

  m_energyEngine.addInputVariable(energyConsumption);
  m_energyEngine.addOutputVariable(energyScore);

  // ── 3 Rules for Energy Consumption
  // ────────────────────────────────────────────────
  // Rule: IF EnergyConsumption is X → EnergyScore is Y
  auto makeRule = [](const QString &term, const QString &score) -> FuzzyRule {
    FuzzyRule r;
    r.antecedents["EnergyConsumption"] = term;
    r.consequent = {"EnergyScore", score};
    return r;
  };
  m_energyEngine.addRule(makeRule("Low", "Excellent"));
  m_energyEngine.addRule(makeRule("Medium", "Good"));
  m_energyEngine.addRule(makeRule("High", "Poor"));
}

// Final score = average of all three sub-scores.
// This gives equal weight to travel time, motor power, and energy consumption.
double OptimizationHandler::evaluateFuzzyScore(double travelTime,
                                               double motorPower,
                                               double energyConsumption) {
  m_timeEngine.setInputValue("TravelTime", travelTime);
  m_powerEngine.setInputValue("MotorPower", motorPower);
  m_energyEngine.setInputValue("EnergyConsumption", energyConsumption);
  const double timeScore = m_timeEngine.getOutputValue("TimeScore");
  const double powerScore = m_powerEngine.getOutputValue("PowerScore");
  const double energyScore = m_energyEngine.getOutputValue("EnergyScore");
  return (timeScore + powerScore + energyScore) / 3.0;
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
    return;
  }

  // Require at least one successful simulation to have valid parameters
  if (m_trainSimulation->getMaxSpeed() <= 0.0) {
    m_isRunning.storeRelaxed(0);
    return;
  }

  // Save and restore user's original parameters
  const double originalAcc = m_movingData->acc_start_si;
  const double originalVp1 = m_movingData->v_p1;

  // Use the caller-supplied candidates directly.
  // Fallback to the user's current parameters if the lists are empty.
  QList<double> accValues =
      accCandidates.isEmpty() ? QList<double>{originalAcc} : accCandidates;
  QList<double> vp1Values =
      vp1Candidates.isEmpty() ? QList<double>{originalVp1} : vp1Candidates;

  m_totalCombinations.storeRelaxed(accValues.size() * vp1Values.size());

  {
    QMutexLocker lk(&m_resultsMutex);
    m_results.clear();
  }

  struct RawEntry {
    double acc, vp1, peakPower, travelTime, energyConsumption;
  };
  QList<RawEntry> rawData;

  for (double acc : accValues) {
    for (double vp1 : vp1Values) {
      // Set parameters and run simulation WITHOUT holding the outer mutex.
      // runDynamicSimulation() acquires m_simulationMutex internally for each
      // step — holding it here too would deadlock (QMutex is non-recursive).
      m_movingData->acc_start_si = acc;
      m_movingData->v_p1 = vp1;

      // Block signals so simulationCompleted is NOT emitted during the sweep.
      // Without this, each runDynamicSimulation() posts a queued
      // resetSimulation() to the main thread (cross-thread signal). Those
      // queued slots can fire mid-loop of the NEXT combo's simulation,
      // corrupting movingData->v, energyData->e_pow, etc. and producing
      // different scores on repeated runs. initData() + clearSimulationDatas()
      // inside runDynamicSimulation() already reset all state we need; we don't
      // need the signal-triggered reset here.
      m_trainSimulation->blockSignals(true);
      m_trainSimulation->runDynamicSimulation();
      m_trainSimulation->blockSignals(false);

      if (m_simulationDatas->powerMotorOutPerMotor.isEmpty() ||
          m_simulationDatas->timeTotal.isEmpty()) {
        continue;
      }

      RawEntry e;
      e.acc = acc;
      e.vp1 = vp1;
      e.peakPower = findMaximumPowerMotorPerCar();
      e.travelTime = m_simulationDatas->timeTotal.last();
      e.energyConsumption = m_simulationDatas->energyConsumptions.last();
      rawData.append(e);
    }
  }

  // Restore user parameters (no mutex needed — simulation is done and we own
  // m_movingData exclusively while m_isRunning == 1).
  m_movingData->acc_start_si = originalAcc;
  m_movingData->v_p1 = originalVp1;

  if (rawData.isEmpty()) {
    m_isRunning.storeRelaxed(0);
    return;
  }

  // ── PASS 2: derive ranges from actual data, calibrate engine, score ─────
  double minT = rawData[0].travelTime, maxT = rawData[0].travelTime;
  double minP = rawData[0].peakPower, maxP = rawData[0].peakPower;
  double minE = rawData[0].energyConsumption, maxE = rawData[0].energyConsumption;
  for (const RawEntry &e : rawData) {
    minT = std::min(minT, e.travelTime);
    maxT = std::max(maxT, e.travelTime);
    minP = std::min(minP, e.peakPower);
    maxP = std::max(maxP, e.peakPower);
    minE = std::min(minE, e.energyConsumption);
    maxE = std::max(maxE, e.energyConsumption);
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
  if (maxE - minE < 1.0) {
    minE -= 1.0;
    maxE += 1.0;
  }

  setupTimeEngine(minT, maxT);
  setupPowerEngine(minP, maxP);
  setupEnergyEngine(minE, maxE);

  {
    QMutexLocker lk(&m_resultsMutex);
    for (const RawEntry &e : rawData) {
      OptResult r;
      r.acc_start_si = e.acc;
      r.v_p1 = e.vp1;
      r.peakMotorPower = e.peakPower;
      r.travelTime = e.travelTime;
      r.energyConsumption = e.energyConsumption;
      r.fuzzyScore =
          evaluateFuzzyScore(e.travelTime, e.peakPower, e.energyConsumption);
      m_results.append(r);
    }

    m_bestResult =
        *std::max_element(m_results.begin(), m_results.end(),
                          [](const OptResult &a, const OptResult &b) {
                            return a.fuzzyScore < b.fuzzyScore;
                          });
  }

  emit optimizationComplete(m_bestResult);
  m_isRunning.storeRelaxed(0);
}

double OptimizationHandler::findMaximumPowerMotorPerCar() {
  if (m_simulationDatas->powerMotorOutPerMotor.isEmpty())
    return 0.0;
  return *std::max_element(m_simulationDatas->powerMotorOutPerMotor.begin(),
                           m_simulationDatas->powerMotorOutPerMotor.end());
}
