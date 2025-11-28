#include "optimization_handler.h"
#include <QCoreApplication>
#include <QDebug>

OptimizationHandler::OptimizationHandler(QObject *parent)
    : QObject(parent), m_isRunning(false), m_workerThread(nullptr) {
  m_stopRequested = 0;
  m_currentResult.iterationCount = 0;
  m_currentResult.suitabilityScore = 0.0;
  m_currentResult.suitabilityLabel = "Not Started";
  m_currentResult.optimizedTrain = {0};
  m_currentResult.scoreHistory.clear();
  qDebug() << "🔧 OptimizationHandler Constructed. Iteration:"
           << m_currentResult.iterationCount;
  setupFuzzyEngine();
}

OptimizationHandler::~OptimizationHandler() {
  stopOptimization();
  if (m_workerThread) {
    m_workerThread->quit();
    m_workerThread->wait();
    delete m_workerThread;
  }
}

void OptimizationHandler::setupFuzzyEngine() {
  m_fuzzyEngine = std::make_unique<FuzzyEngine>();

  // 1. Inputs
  // Acceleration (m/s^2)
  auto acc = std::make_shared<FuzzyVariable>("Acceleration", 0.0, 3.0);
  acc->addTerm(std::make_shared<TriangleSet>("Low", 0.0, 0.0, 0.8));
  acc->addTerm(std::make_shared<TriangleSet>("Medium", 0.5, 1.0, 1.5));
  acc->addTerm(std::make_shared<TriangleSet>("High", 1.2, 3.0, 3.0));
  m_fuzzyEngine->addInputVariable(acc);

  // Weakening Point (km/h)
  auto wp = std::make_shared<FuzzyVariable>("WeakeningPoint", 0.0, 120.0);
  wp->addTerm(std::make_shared<TriangleSet>("Low", 0.0, 0.0, 35.0));
  wp->addTerm(std::make_shared<TriangleSet>("Medium", 30.0, 45.0, 60.0));
  wp->addTerm(std::make_shared<TriangleSet>("High", 50.0, 120.0, 120.0));
  m_fuzzyEngine->addInputVariable(wp);

  // Gradient (permil)
  auto grad = std::make_shared<FuzzyVariable>("Gradient", 0.0, 40.0);
  grad->addTerm(std::make_shared<TriangleSet>("Low", 0.0, 0.0, 5.0));
  grad->addTerm(std::make_shared<TriangleSet>("Medium", 2.0, 10.0, 20.0));
  grad->addTerm(std::make_shared<TriangleSet>("High", 15.0, 40.0, 40.0));
  m_fuzzyEngine->addInputVariable(grad);

  // Speed Limit (km/h)
  auto speed = std::make_shared<FuzzyVariable>("SpeedLimit", 0.0, 160.0);
  speed->addTerm(std::make_shared<TriangleSet>("Low", 0.0, 0.0, 60.0));
  speed->addTerm(std::make_shared<TriangleSet>("Medium", 50.0, 80.0, 110.0));
  speed->addTerm(std::make_shared<TriangleSet>("High", 100.0, 160.0, 160.0));
  m_fuzzyEngine->addInputVariable(speed);

  // 2. Output: Suitability
  auto suit = std::make_shared<FuzzyVariable>("Suitability", 0.0, 1.0);
  suit->addTerm(std::make_shared<TriangleSet>("NotSuitable", 0.0, 0.0, 0.4));
  suit->addTerm(std::make_shared<TriangleSet>("QuiteSuitable", 0.3, 0.5, 0.7));
  suit->addTerm(std::make_shared<TriangleSet>("VerySuitable", 0.6, 1.0, 1.0));
  m_fuzzyEngine->addOutputVariable(suit);

  // 3. Rules (Examples from Thesis + Logic)
  // Rule 1: Low Acc + High Grade -> Not Suitable
  m_fuzzyEngine->addRule({{{"Acceleration", "Low"}, {"Gradient", "High"}},
                          {"Suitability", "NotSuitable"}});

  // Rule 2: Medium Acc + Medium WP + Medium Grade -> Quite Suitable
  m_fuzzyEngine->addRule({{{"Acceleration", "Medium"},
                           {"WeakeningPoint", "Medium"},
                           {"Gradient", "Medium"}},
                          {"Suitability", "QuiteSuitable"}});

  // Rule 3: High Acc + High WP -> Very Suitable
  m_fuzzyEngine->addRule(
      {{{"Acceleration", "High"}, {"WeakeningPoint", "High"}},
       {"Suitability", "VerySuitable"}});

  // Rule 4: Low WP + High SpeedLimit -> Not Suitable
  m_fuzzyEngine->addRule({{{"WeakeningPoint", "Low"}, {"SpeedLimit", "High"}},
                          {"Suitability", "NotSuitable"}});

  // Rule 5 (From Thesis Example): WP Medium + Speed Medium + Grad Medium ->
  // Quite Suitable
  m_fuzzyEngine->addRule({{{"WeakeningPoint", "Medium"},
                           {"SpeedLimit", "Medium"},
                           {"Gradient", "Medium"}},
                          {"Suitability", "QuiteSuitable"}});
}

void OptimizationHandler::startOptimization(const TrainData &baseTrain,
                                            const MassData &baseMass,
                                            const SimulationDatas &simData) {
  if (m_isRunning)
    return;

  m_baseTrain = baseTrain;
  m_baseMass = baseMass;
  m_baseSimData = simData;
  m_stopRequested = 0;
  m_isRunning = true;

  // Move to worker thread
  m_workerThread = new QThread;
  this->moveToThread(m_workerThread);

  connect(m_workerThread, &QThread::started, this,
          &OptimizationHandler::runOptimizationLoop);
  connect(this, &OptimizationHandler::optimizationFinished, m_workerThread,
          &QThread::quit);
  connect(m_workerThread, &QThread::finished, m_workerThread,
          &QThread::deleteLater);

  m_workerThread->start();
  std::cerr << "🚀 Optimization Thread Started" << std::endl;
  emit optimizationStarted();
}

void OptimizationHandler::stopOptimization() { m_stopRequested = 1; }

void OptimizationHandler::runOptimizationLoop() {
  TrainData currentCandidate = m_baseTrain;
  TrainData bestCandidate = m_baseTrain;
  double bestScore = -1.0;
  int maxIterations = 50;

  m_currentResult.scoreHistory.clear();

  for (int i = 0; i < maxIterations; ++i) {
    if (m_stopRequested)
      break;

    // 1. Run Simulation (Headless)
    SimMetrics metrics =
        runHeadlessSimulation(currentCandidate, m_baseMass, m_baseSimData);

    // 2. Evaluate (The Judge)
    m_fuzzyEngine->setInputValue("Acceleration", metrics.acceleration);
    m_fuzzyEngine->setInputValue("WeakeningPoint", metrics.weakeningPoint);
    m_fuzzyEngine->setInputValue("Gradient", metrics.maxGradient);
    m_fuzzyEngine->setInputValue("SpeedLimit", metrics.speedLimit);

    double score = m_fuzzyEngine->getOutputValue("Suitability");

    std::cerr << "Iter " << i << ": Acc=" << metrics.acceleration
              << " WP=" << metrics.weakeningPoint
              << " Grad=" << metrics.maxGradient
              << " Speed=" << metrics.speedLimit << " -> Score=" << score
              << std::endl;

    // Update Result
    {
      QMutexLocker locker(&m_resultMutex);
      m_currentResult.scoreHistory.push_back(score);
      m_currentResult.debug_acc = metrics.acceleration;
      m_currentResult.debug_wp = metrics.weakeningPoint;
      m_currentResult.debug_grad = metrics.maxGradient;
      m_currentResult.debug_speed = metrics.speedLimit;
      if (score > bestScore) {
        bestScore = score;
        bestCandidate = currentCandidate;
        m_currentResult.optimizedTrain = bestCandidate;
        m_currentResult.suitabilityScore = bestScore;
      }
    }

    emit optimizationProgress(i + 1, score, bestScore);

    // 3. Stop if good enough
    if (score >= 0.85)
      break;

    // 4. Adjust (The Mechanic)
    // Find out WHY it's bad
    QString accTerm = m_fuzzyEngine->getDominantInputTerm("Acceleration");
    QString wpTerm = m_fuzzyEngine->getDominantInputTerm("WeakeningPoint");

    QString deficit = "";
    if (accTerm == "Low")
      deficit = "LowAcceleration";
    else if (wpTerm == "Low")
      deficit = "LowWeakening";

    adjustCandidate(currentCandidate, deficit);

    // Sleep slightly to not freeze UI if running on main thread (though we are
    // on worker)
    QThread::msleep(50);
  }

  m_isRunning = false;
  emit optimizationFinished(m_currentResult);
}

OptimizationHandler::SimMetrics
OptimizationHandler::runHeadlessSimulation(const TrainData &train,
                                           const MassData &mass,
                                           const SimulationDatas &simData) {
  // Simplified simulation for optimization
  // In a real scenario, this would call TrainSimulationHandler methods.
  // For now, we approximate metrics based on physics formulas to be fast.

  SimMetrics metrics;

  // Calculate Acceleration (F = ma)
  // Tractive Effort approx = (Power / Speed) * Efficiency
  // But at start, TE is limited by adhesion or motor max torque.
  // Let's use a simplified model:

  double totalMass = mass.mass_totalLoad * 1000; // kg
  if (totalMass < 1000.0) {
    // Fallback: Estimate mass if totalLoad is not set
    // Approx 35 tons per car (empty + load)
    totalMass = train.n_car * 35.0 * 1000;
  }

  double totalPower = train.n_tm * 190 * 1000; // Assuming 190kW per motor
  double force = (totalPower / 15.0);          // Force at low speed (approx)
  force = force * train.gearRatio / 5.0;       // Adjust by gear ratio

  metrics.acceleration = force / totalMass;

  // Weakening Point approx
  // Speed where voltage limit is hit. Proportional to Wheel Diameter / Gear
  // Ratio
  metrics.weakeningPoint =
      (train.wheel * 3.14 * 60 * 1000) / (train.gearRatio * 1000);
  // Normalize to reasonable range
  metrics.weakeningPoint =
      35.0 * (850.0 / train.wheel) * (6.0 / train.gearRatio);

  // Track Data (Constant for this track)
  metrics.maxGradient = 5.0; // Placeholder, should come from SimulationData
  metrics.speedLimit = 80.0; // Placeholder

  return metrics;
}

void OptimizationHandler::adjustCandidate(TrainData &candidate,
                                          const QString &dominantDeficit) {
  // The Mechanic Logic
  if (dominantDeficit == "LowAcceleration") {
    // Need more torque: Increase Gear Ratio OR More Motors
    if (candidate.gearRatio < 10.0) {
      candidate.gearRatio += 0.2;
    } else {
      candidate.n_tm += 1;
    }
  } else if (dominantDeficit == "LowWeakening") {
    // Need more speed range: Decrease Gear Ratio
    if (candidate.gearRatio > 3.0) {
      candidate.gearRatio -= 0.2;
    }
  } else {
    // Random mutation if no clear deficit (Exploration)
    if (rand() % 2 == 0)
      candidate.gearRatio += ((rand() % 10) - 5) * 0.05;
    else
      candidate.n_tm += (rand() % 3) - 1;
  }

  // Bounds check
  if (candidate.n_tm < 1)
    candidate.n_tm = 1;
  if (candidate.gearRatio < 1.0)
    candidate.gearRatio = 1.0;
}

#include <iostream>

OptimizationResult OptimizationHandler::getResult() const {
  QMutexLocker locker(&m_resultMutex);
  std::cerr << "🔍 getResult called on " << this
            << ". Iteration: " << m_currentResult.iterationCount << std::endl;
  return m_currentResult;
}
