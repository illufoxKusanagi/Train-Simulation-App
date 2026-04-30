#ifndef OPTIMIZATION_HANDLER_H
#define OPTIMIZATION_HANDLER_H

#include "controllers/optimization/fuzzy/fuzzy_engine.h"
#include "controllers/simulation/train_simulation_handler.h"
#include "core/appcontext.h"
#include "models/moving_data.h"
#include "models/simulation_data.h"
#include <QAtomicInt>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

struct OptResult {
  double acc_start_si;
  double v_p1;
  double peakMotorPower;
  double travelTime;
  double fuzzyScore;
  double energyConsumption;
};

class OptimizationHandler : public QObject {
  Q_OBJECT

public:
  explicit OptimizationHandler(AppContext *context,
                               TrainSimulationHandler *simulationHandler,
                               QObject *parent = nullptr);
  void handleOptimization(const QList<double> &accCandidates,
                          const QList<double> &vp1Candidates);

  QList<OptResult> getResults() const {
    QMutexLocker lk(&m_resultsMutex);
    return m_results;
  }
  OptResult getBestResult() const {
    QMutexLocker lk(&m_resultsMutex);
    return m_bestResult;
  }
  bool isRunning() const { return m_isRunning.loadRelaxed() == 1; }
  int getTotalCombinations() const { return m_totalCombinations.loadRelaxed(); }

signals:
  void optimizationComplete(OptResult best);

private:
  TrainSimulationHandler *m_trainSimulation;
  MovingData *m_movingData;
  SimulationDatas *m_simulationDatas;
  QMutex *m_simulationMutex;
  FuzzyEngine m_timeEngine;
  FuzzyEngine m_powerEngine;
  FuzzyEngine m_energyEngine;

  mutable QMutex m_resultsMutex;
  QList<OptResult> m_results;
  OptResult m_bestResult;
  QAtomicInt m_isRunning;
  QAtomicInt m_totalCombinations;

  void setupTimeEngine(double minT, double maxT);
  void setupPowerEngine(double minP, double maxP);
  void setupEnergyEngine(double minP, double maxP);

  double evaluateFuzzyScore(double travelTime, double motorPower,
                            double energyConsumption);

  double findMaximumPowerMotorPerCar();
};

#endif // OPTIMIZATION_HANDLER_H
