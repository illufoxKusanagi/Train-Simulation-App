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
#include <QObject>
#include <QThread>
#include <memory>
#include <qmutex.h>
#include <qobject.h>

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
  QList<OptResult> getResults() const { return m_results; }
  OptResult getBestResult() const { return m_bestResult; }
  bool isRunning() const { return m_isRunning.loadRelaxed() == 1; }

signals:
  void optimizationComplete(OptResult best);

private:
  TrainSimulationHandler *m_trainSimulation;
  MovingData *m_movingData;
  SimulationDatas *m_simulationDatas;
  QMutex *m_simulationMutex;
  FuzzyEngine m_fuzzyEngine;

  // All 20 combo results — persists after handleOptimization() returns
  QList<OptResult> m_results;
  OptResult m_bestResult;
  QAtomicInt m_isRunning; // 1 = running, 0 = idle

  // Build Travel Time + Motor Power inputs, Fuzzy Score output, and 9 rules.
  // Ranges are derived from the actual sweep results so any train/track works.
  void setupFuzzyEngine(double minT, double maxT, double minP, double maxP);

  // Feed travelTime + motorPower into the engine, return centroid score 0–100
  double evaluateFuzzyScore(double travelTime, double motorPower);

  // Peak motor power from the last simulation run (more meaningful than avg)
  double findMaximumPowerMotorPerCar();
};

#endif // OPTIMIZATION_HANDLER_H
