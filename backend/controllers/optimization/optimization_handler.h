#ifndef OPTIMIZATION_HANDLER_H
#define OPTIMIZATION_HANDLER_H

#include "controllers/simulation/train_simulation_handler.h"
#include "core/appcontext.h"
#include "models/mass_data.h"
#include "models/moving_data.h"
#include "models/simulation_data.h"
#include "models/station_data.h"
#include "models/train_data.h"
#include "utils/fuzzy_engine.h"
#include <QAtomicInt>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <memory>

// One result entry per (acc_start, v_p1) combination
struct OptResult {
  double acc_start; // m/s²
  double v_p1;      // km/h — field weakening point 1 (powering)
  double
      peakMotorPower; // kW/motor — peak during full-power phase (fuzzy input 1)
  double travelTime;  // seconds  — total trip duration (fuzzy input 2)
  double fuzzyScore;  // 0–100 (centroid defuzzified)
};

class OptimizationHandler : public QObject {
  Q_OBJECT

public:
  explicit OptimizationHandler(AppContext *context,
                               TrainSimulationHandler *simulationHandler,
                               QObject *parent = nullptr);
  void handleOptimization();

  // Access results after optimization completes
  QList<OptResult> getResults() const {
    QMutexLocker lk(&m_resultsMutex);
    return m_results;
  }
  OptResult getBestResult() const {
    QMutexLocker lk(&m_resultsMutex);
    return m_bestResult;
  }
  bool isRunning() const { return m_isRunning.loadRelaxed() == 1; }

signals:
  void optimizationComplete(OptResult best);

private:
  TrainSimulationHandler *m_trainSimulation;
  MovingData *m_movingData;
  SimulationDatas *m_simulationDatas;
  QMutex *m_simulationMutex;
  // Two separate engines — one per optimized parameter.
  // m_timeEngine  : evaluates TravelTime  (driven by acc_start)
  // m_powerEngine : evaluates MotorPower  (driven by v_p1)
  FuzzyEngine m_timeEngine;
  FuzzyEngine m_powerEngine;

  mutable QMutex m_resultsMutex; // guards m_results and m_bestResult
  // All sweep combo results — persists after handleOptimization() returns
  QList<OptResult> m_results;
  OptResult m_bestResult;
  QAtomicInt m_isRunning; // 1 = running, 0 = idle

  // ── Per-parameter engine setup ──────────────────────────────────────────
  // Each function sets up ONE engine independently:
  //   setupTimeEngine  : TravelTime input → TimeScore output  (3 rules)
  //   setupPowerEngine : MotorPower input → PowerScore output (3 rules)
  // Ranges come from actual Pass 1 data so they adapt to any configuration.
  void setupTimeEngine(double minT, double maxT);
  void setupPowerEngine(double minP, double maxP);

  // Run both engines and return the average of TimeScore and PowerScore (0–100)
  double evaluateFuzzyScore(double travelTime, double motorPower);

  // Peak motor power from the last simulation run (more meaningful than avg)
  double findMaximumPowerMotorPerCar();
};

#endif // OPTIMIZATION_HANDLER_H
