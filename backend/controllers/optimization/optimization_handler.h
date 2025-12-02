#ifndef OPTIMIZATION_HANDLER_H
#define OPTIMIZATION_HANDLER_H

#include "controllers/simulation/train_simulation_handler.h"
#include "models/mass_data.h"
#include "models/simulation_data.h"
#include "models/train_data.h"
#include "utils/fuzzy_engine.h"
#include <QAtomicInt>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <memory>

// Struct to hold optimization results
struct OptimizationResult {
  TrainData optimizedTrain;
  double suitabilityScore;
  QString suitabilityLabel;
  int iterationCount;
  QVector<double> scoreHistory;
  double debug_acc;
  double debug_wp;
  double debug_grad;
  double debug_speed;
};

class OptimizationHandler : public QObject {
  Q_OBJECT

public:
  explicit OptimizationHandler(QObject *parent = nullptr);
  ~OptimizationHandler();

  // Main control methods
  void startOptimization(const TrainData &baseTrain, const MassData &baseMass,
                         const SimulationDatas &simData);
  void stopOptimization();
  void applyOptimization(); // Emits signal to update main AppContext

  // Status
  bool isRunning() const { return m_isRunning; }
  OptimizationResult getResult() const;

signals:
  void optimizationStarted();
  void optimizationFinished(const OptimizationResult &result);
  void optimizationProgress(int iteration, double currentScore,
                            double bestScore);
  void optimizationError(const QString &message);

private slots:
  void runOptimizationLoop();

private:
  // The "Judge"
  void setupFuzzyEngine();
  double evaluateCandidate(const TrainData &candidate,
                           const SimulationDatas &simData);

  // The "Mechanic"
  void adjustCandidate(TrainData &candidate, const QString &dominantDeficit);

  // Helper to run a headless simulation
  // Returns key metrics: Acc, WeakeningPoint, MaxSpeed, etc.
  struct SimMetrics {
    double acceleration;
    double weakeningPoint;
    double maxGradient; // From track data
    double speedLimit;  // From track data
  };
  SimMetrics runHeadlessSimulation(const TrainData &train, const MassData &mass,
                                   const SimulationDatas &simData);

  // State
  QAtomicInt m_stopRequested;
  bool m_isRunning;
  OptimizationResult m_currentResult;
  mutable QMutex m_resultMutex;

  // Components
  std::unique_ptr<FuzzyEngine> m_fuzzyEngine;

  // Threading
  QThread *m_workerThread;

  // Data copies for the worker thread
  TrainData m_baseTrain;
  MassData m_baseMass;
  SimulationDatas m_baseSimData;
};

#endif // OPTIMIZATION_HANDLER_H
