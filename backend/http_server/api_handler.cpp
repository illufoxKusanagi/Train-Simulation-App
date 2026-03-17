#include "api_handler.h"
#include <QDateTime>
#include <QDebug>
#include <QHttpServerResponse>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtConcurrent>

ApiHandler::ApiHandler(AppContext &context, QObject *parent)
    : QObject(parent), m_context(context) {
  // Authentication
  m_userManager = new UserManager(this);
  m_authManager = new AuthManager(m_userManager, this);

  // Sub handlers
  m_trainHandler = new TrainParameterHandler(context, this);
  m_electricalHandler = new ElectricalParameterHandler(context, this);
  m_runningHandler = new RunningParameterHandler(context, this);
  m_trackHandler = new TrackParameterHandler(context, this);
  m_simulationHandler = new SimulationHandler(context, this);
  m_optimizationHandler = new OptimizationHandler(
      &context, m_simulationHandler->getTrainSimulation(), this);
  // m_exportHandler = new ExportHandler(context, this);

  // **FIX: Initialize handlers safely with null checks**
  m_trainDataHandler = new TrainDataHandler(&context, this);
  m_electricalDataHandler = new ElectricalDataHandler(&context, this);
  m_runningDataHandler = new RunningDataHandler(&context, this);
  m_trackDataHandler = new TrackDataHandler(&context, this);
  // m_simulationHandler = new TrainSimulationHandler(context, this);

  // **FIX: Check if simulationDatas exists before creating CSV handler**
  if (context.simulationDatas) {
    m_csvOutputHandler = new CsvOutputHandler(*context.simulationDatas);
  } else {
    m_csvOutputHandler = nullptr;
    qWarning()
        << "simulationDatas not initialized, CSV export will not be available";
  }
}

QHttpServerResponse ApiHandler::handleHealthCheck() {
  QJsonObject response;
  response["status"] = "ok";
  response["service"] = "Train Simulation Backend";
  response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
  response["dataStatus"] =
      QJsonObject{{"trainData", m_context.trainData != nullptr},
                  {"powerData", m_context.powerData != nullptr},
                  {"efficiencyData", m_context.efficiencyData != nullptr},
                  {"loadData", m_context.loadData != nullptr},
                  {"massData", m_context.massData != nullptr},
                  {"energyData", m_context.energyData != nullptr},
                  {"simulationDatas", m_context.simulationDatas != nullptr}};

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleLogin(const QJsonObject &data) {
  QJsonObject response;

  QString username = data.value("username").toString();
  QString password = data.value("password").toString();

  if (m_authManager->login(username, password)) {
    response["status"] = "success";
    response["message"] = "Login successful";
    response["token"] = "sim-auth-token";
    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
  } else {
    response["status"] = "error";
    response["message"] = "Invalid username or password";
    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Unauthorized);
  }
}

QHttpServerResponse ApiHandler::handleGetTrainParameters() {
  return m_trainHandler->handleGetTrainParameters();
}

QHttpServerResponse
ApiHandler::handleUpdateTrainParameters(const QJsonObject &data) {
  return m_trainHandler->handleUpdateTrainParameters(data);
}

QHttpServerResponse ApiHandler::handleExportResults(const QJsonObject &data) {
  QJsonObject response;

  // **FIX: Check if CSV handler exists**
  if (!m_csvOutputHandler) {
    response["status"] = "error";
    response["message"] =
        "CSV export not available - simulation data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    QString format = data.value("format").toString("csv");

    if (format == "csv") {
      m_csvOutputHandler->printSimulationDatas();

      response["status"] = "success";
      response["message"] = "Results exported to CSV successfully";
    } else {
      response["status"] = "error";
      response["message"] = "Unsupported export format";
    }
  } catch (const std::exception &e) {
    response["status"] = "error";
    response["message"] = QString("Error exporting results: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

// **FIX: Similar null checks for other methods**
QHttpServerResponse ApiHandler::handleGetElectricalParameters() {
  return m_electricalHandler->handleGetElectricalParameters();
}

QHttpServerResponse
ApiHandler::handleUpdateElectricalParameters(const QJsonObject &data) {
  return m_electricalHandler->handleUpdateElectricalParameters(data);
}

QHttpServerResponse ApiHandler::handleGetRunningParameters() {
  return m_runningHandler->handleGetRunningParameters();
}

QHttpServerResponse
ApiHandler::handleUpdateRunningParameters(const QJsonObject &data) {
  return m_runningHandler->handleUpdateRunningParameters(data);
}

QHttpServerResponse ApiHandler::handleGetTrackParameters() {
  return m_trackHandler->handleGetTrackParameters();
}

QHttpServerResponse
ApiHandler::handleUpdateTrackParameters(const QJsonObject &data) {
  return m_trackHandler->handleUpdateTrackParameters(data);
}

QHttpServerResponse ApiHandler::handleGetCarNumberParameters() {
  return m_trainHandler->handleGetCarNumberParameters();
}

QHttpServerResponse
ApiHandler::handleUpdateCarNumberParameters(const QJsonObject &data) {
  return m_trainHandler->handleUpdateCarNumberParameters(data);
}

QHttpServerResponse ApiHandler::handleGetPassengerParameters() {
  return m_trainHandler->handleGetPassengerParameters();
}

QHttpServerResponse
ApiHandler::handleUpdatePassengerParameters(const QJsonObject &data) {
  return m_trainHandler->handleUpdatePassengerParameters(data);
}

QHttpServerResponse ApiHandler::handleGetMassParameters() {
  return m_trainHandler->handleGetMassParameters();
}

QHttpServerResponse
ApiHandler::handleUpdateMassParameters(const QJsonObject &data) {
  return m_trainHandler->handleUpdateMassParameters(data);
}

QHttpServerResponse ApiHandler::handleStartSimulation(const QJsonObject &data) {
  return m_simulationHandler->handleStartSimulation(data);
}

QHttpServerResponse ApiHandler::handleGetSimulationStatus() {
  return m_simulationHandler->handleGetSimulationStatus();
}

QHttpServerResponse ApiHandler::handleGetSimulationResults() {
  return m_simulationHandler->handleGetSimulationResults();
}

QHttpServerResponse ApiHandler::handleQuickInit() {
  QJsonObject response;

  try {
    qDebug() << "🔧 Quick initialization with minimal valid data";

    // Train parameters - MATCHING FRONTEND DEFAULT VALUES
    if (m_context.trainData) {
      m_context.trainData->n_tm = 24.0;      // Number of traction motors
      m_context.trainData->n_axle = 4.0;     // Axles per car
      m_context.trainData->n_car = 12.0;     // Number of cars (12-car default)
      m_context.trainData->gearRatio = 6.53; // Gear ratio
      m_context.trainData->wheel = 860.0;    // Wheel diameter (mm)
      m_context.trainData->trainsetLength =
          20.0 * m_context.trainData->n_car; // 12 cars × 20m

      // Frontend defaults (trainset)
      m_context.trainData->n_Tc = 2.0;
      m_context.trainData->n_M1 = 3.0;
      m_context.trainData->n_M2 = 3.0;
      m_context.trainData->n_T1 = 2.0;
      m_context.trainData->n_T2 = 1.0;
      m_context.trainData->n_T3 = 1.0;
      m_context.trainData->n_M1_disabled = 0.0;
      m_context.trainData->n_M2_disabled = 0.0;
      qDebug() << "✓ Train parameters initialized (matching frontend defaults)";
    }

    // Mass parameters - MATCHING FRONTEND DEFAULT VALUES
    if (m_context.massData) {
      m_context.massData->mass_M1 = 37.5;
      m_context.massData->mass_M2 = 36.72;
      m_context.massData->mass_TC = 34.48;
      m_context.massData->mass_T1 = 33.335;
      m_context.massData->mass_T2 = 30.05;
      m_context.massData->mass_T3 = 29.66;
      m_context.massData->i_M = 1.1;
      m_context.massData->i_T = 1.05;
      qDebug() << "✓ Mass parameters initialized (matching frontend defaults)";
    }

    // Load parameters - MATCHING FRONTEND DEFAULT VALUES
    if (m_context.loadData) {
      m_context.loadData->load = 0.0;
      m_context.loadData->mass_P = 70.0;
      m_context.loadData->mass_P_final =
          m_context.loadData->mass_P / 1000; // 70kg / 1000

      m_context.loadData->n_PM1 = 289.0;
      m_context.loadData->n_PM2 = 289.0;
      m_context.loadData->n_PTc = 253.0;
      m_context.loadData->n_PT1 = 289.0;
      m_context.loadData->n_PT2 = 289.0;
      m_context.loadData->n_PT3 = 289.0;

      qDebug() << "✓ Load parameters initialized (matching frontend defaults)";
    }

    // Motor data (realistic values for 8-car EMU with 24 motors)
    // if (m_context.trainMotorData) {
    //   m_context.trainMotorData->tm_f =
    //       250.0; // Total traction force (kN) - much higher for realistic
    //              // acceleration
    //   m_context.trainMotorData->tm_adh = 0.25; // Adhesion coefficient
    //   qDebug() << "✓ Motor parameters initialized";
    // }

    // Efficiency parameters
    if (m_context.efficiencyData) {
      m_context.efficiencyData->stat_eff_gear = 0.98;
      m_context.efficiencyData->stat_eff_motor = 0.89;
      m_context.efficiencyData->stat_eff_vvvf = 0.97;
      qDebug() << "✓ Efficiency parameters initialized";
    }

    // Resistance parameters
    if (m_context.resistanceData) {
      m_context.resistanceData->startRes = 39.2;
      m_context.resistanceData->slope = 0.0;
      m_context.resistanceData->radius = 2000.0;
      qDebug() << "✓ Resistance parameters initialized";
    }

    // Station data - MATCHING FRONTEND DEFAULT VALUES
    if (m_context.stationData) {
      m_context.stationData->stat_slope_option1 = 0.0;
      m_context.stationData->stat_slope_option2 = 5.0;
      m_context.stationData->stat_slope_option3 = 10.0;
      m_context.stationData->stat_slope_option4 = 25.0;
      m_context.stationData->stat_radius = 2000.0;
      m_context.stationData->stat_x_station = 2000.0; // 2km
      m_context.stationData->stat_v_limit = 80.0;
      m_context.stationData->stat_dwellTime = 60.0;
      m_context.stationData->n_station = 2;
      qDebug() << "✓ Station parameters initialized";
    }

    // Running parameters
    if (m_context.movingData) {
      m_context.movingData->v = 0.0;
      m_context.movingData->v_diffCoast = 5.0;
      m_context.movingData->acc_start = 0.8;
      m_context.movingData->v_p1 = 35.0;
      m_context.movingData->v_p2 = 65.0;
      m_context.movingData->v_b1 = 55.0;
      m_context.movingData->v_b2 = 70.0;
      m_context.movingData->decc_start = 1.0;
      m_context.movingData->decc_emergency = 1.2;
      qDebug() << "✓ Running parameters initialized";
    }

    if (m_context.energyData) {
      m_context.energyData->stat_vol_line = 1500;
      m_context.energyData->stat_vol_motor = 1200;
    }

    if (m_context.powerData) {
      m_context.powerData->p_aps = 30;
    }

    response["status"] = "success";
    response["message"] =
        "Quick initialization completed with minimal valid values";
    response["initialized"] =
        QJsonObject{{"trainData", m_context.trainData != nullptr},
                    {"massData", m_context.massData != nullptr},
                    {"loadData", m_context.loadData != nullptr},
                    {"motorData", m_context.trainMotorData != nullptr},
                    {"efficiencyData", m_context.efficiencyData != nullptr},
                    {"resistanceData", m_context.resistanceData != nullptr},
                    {"stationData", m_context.stationData != nullptr},
                    {"movingData", m_context.movingData != nullptr}};

    // Log actual values for verification
    qDebug() << "📋 Verification - Train Data:";
    qDebug() << "  n_tm:" << m_context.trainData->n_tm;
    qDebug() << "  n_car:" << m_context.trainData->n_car;
    qDebug() << "  wheel:" << m_context.trainData->wheel;
    qDebug() << "📋 Verification - Mass Data:";
    qDebug() << "  mass_totalEmpty:" << m_context.massData->mass_totalEmpty;
    qDebug() << "  mass_totalInertial:"
             << m_context.massData->mass_totalInertial;
    qDebug() << "📋 Verification - Station Data:";
    qDebug() << "  stat_x_station:" << m_context.stationData->stat_x_station;
    qDebug() << "  stat_v_limit:" << m_context.stationData->stat_v_limit;
    qDebug() << "📋 Verification - Moving Data:";
    qDebug() << "  acc_start:" << m_context.movingData->acc_start;
    qDebug() << "  v_diffCoast:" << m_context.movingData->v_diffCoast;

    qDebug() << "🎉 Quick initialization successful - simulation ready!";
  } catch (const std::exception &e) {
    qCritical() << "💥 Quick init failed:" << e.what();
    response["status"] = "error";
    response["message"] = QString("Initialization error: %1").arg(e.what());
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleDebugContext() {
  QJsonObject response;
  QJsonObject trainData;
  QJsonObject massData;
  QJsonObject movingData;
  QJsonObject stationData;

  // Collect current values from AppContext
  if (m_context.trainData) {
    trainData["n_tm"] = m_context.trainData->n_tm;
    trainData["n_car"] = m_context.trainData->n_car;
    trainData["n_M1"] = m_context.trainData->n_M1;
    trainData["n_M2"] = m_context.trainData->n_M2;
    trainData["n_Tc"] = m_context.trainData->n_Tc;
    trainData["n_T1"] = m_context.trainData->n_T1;
    trainData["wheel"] = m_context.trainData->wheel;
    trainData["gearRatio"] = m_context.trainData->gearRatio;
  }

  if (m_context.massData) {
    massData["mass_M1"] = m_context.massData->mass_M1;
    massData["mass_M2"] = m_context.massData->mass_M2;
    massData["mass_TC"] = m_context.massData->mass_TC;
    massData["mass_T1"] = m_context.massData->mass_T1;
    massData["mass_Me"] = m_context.massData->mass_Me;
    massData["mass_Te"] = m_context.massData->mass_Te;
    massData["mass_totalEmpty"] = m_context.massData->mass_totalEmpty;
    massData["mass_totalInertial"] = m_context.massData->mass_totalInertial;
    massData["i_M"] = m_context.massData->i_M;
    massData["i_T"] = m_context.massData->i_T;
  }

  if (m_context.movingData) {
    movingData["acc_start"] = m_context.movingData->acc_start;
    movingData["decc_start"] = m_context.movingData->decc_start;
    movingData["v"] = m_context.movingData->v;
    movingData["acc"] = m_context.movingData->acc;
  }

  if (m_context.stationData) {
    stationData["stat_x_station"] = m_context.stationData->stat_x_station;
    stationData["stat_v_limit"] = m_context.stationData->stat_v_limit;
    stationData["stat_dwellTime"] = m_context.stationData->stat_dwellTime;
  }

  response["trainData"] = trainData;
  response["massData"] = massData;
  response["movingData"] = movingData;
  response["stationData"] = stationData;
  response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

  qDebug() << "🐛 Debug context exported";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleCalculateMass(const QJsonObject &data) {
  return m_trainHandler->handleCalculateMass(data);
}

ApiHandler::~ApiHandler() {
  if (m_optimizationFuture.isRunning())
    m_optimizationFuture.waitForFinished();
}

QHttpServerResponse
ApiHandler::handleStartOptimization(const QJsonObject &data) {
  QJsonObject response;
  // Guard: require a successful simulation before optimizing
  if (m_simulationHandler->getTrainSimulation()->getMaxSpeed() <= 0.0) {
    response["status"] = "error";
    response["message"] = "Run a dynamic simulation first before optimizing";
    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::BadRequest);
  }

  // Build sweep candidates from the request body.
  // accelLow → accelHigh swept in 0.05 m/s² steps (acc_start candidates)
  // weakeningLow → weakeningHigh swept in 5 km/h steps (v_p1 candidates)
  // Falls back to the user's current parameters when fields are absent.
  QList<double> accValues;
  QList<double> vp1Values;

  const double accLow =
      data.contains("accelLow") ? data["accelLow"].toDouble() : 0.0;
  const double accHigh =
      data.contains("accelHigh") ? data["accelHigh"].toDouble() : 0.0;
  if (accLow > 0.0 && accHigh >= accLow) {
    constexpr double kAccStep = 0.05; // m/s²
    const int steps = qRound((accHigh - accLow) / kAccStep);
    for (int i = 0; i <= steps; ++i)
      accValues.append(accLow + i * kAccStep);
  }

  const double vp1Low =
      data.contains("weakeningLow") ? data["weakeningLow"].toDouble() : 0.0;
  const double vp1High =
      data.contains("weakeningHigh") ? data["weakeningHigh"].toDouble() : 0.0;
  if (vp1Low > 0.0 && vp1High >= vp1Low) {
    constexpr double kVp1Step = 5.0; // km/h
    const int steps = qRound((vp1High - vp1Low) / kVp1Step);
    for (int i = 0; i <= steps; ++i)
      vp1Values.append(vp1Low + i * kVp1Step);
  }

  const int nAcc = accValues.isEmpty() ? 1 : accValues.size();
  const int nVp1 = vp1Values.isEmpty() ? 1 : vp1Values.size();

  // Guard against concurrent optimization runs: check BEFORE dispatching the
  // background task so the caller receives an immediate 409 Conflict rather
  // than silently starting a second run that is blocked by the inner atomic.
  if (m_optimizationHandler->isRunning()) {
    response["status"] = "error";
    response["message"] = "Optimization is already running";
    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Conflict);
  }

  // Run on background thread — handleOptimization() is blocking.
  // The atomic inside handleOptimization() additionally guards re-entry.
  m_optimizationFuture = QtConcurrent::run([this, accValues, vp1Values]() {
    m_optimizationHandler->handleOptimization(accValues, vp1Values);
  });
  response["status"] = "success";
  response["message"] =
      QString("Optimization started (%1 combinations: %2 acc × %3 v_p1)")
          .arg(nAcc * nVp1)
          .arg(nAcc)
          .arg(nVp1);
  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleGetOptimizationStatus() {
  QJsonObject response;
  response["isRunning"] = m_optimizationHandler->isRunning();

  // All sweep results as a JSON array
  QJsonArray resultsArray;
  for (const OptResult &r : m_optimizationHandler->getResults()) {
    QJsonObject row;
    row["acc_start"] = r.acc_start;
    row["v_p1"] = r.v_p1;
    row["peakMotorPower"] = r.peakMotorPower;
    row["travelTime"] = r.travelTime;
    row["fuzzyScore"] = r.fuzzyScore;
    resultsArray.append(row);
  }
  response["results"] = resultsArray;

  // Best result (empty object while running)
  if (!m_optimizationHandler->isRunning() &&
      !m_optimizationHandler->getResults().isEmpty()) {
    OptResult best = m_optimizationHandler->getBestResult();
    QJsonObject bestObj;
    bestObj["acc_start"] = best.acc_start;
    bestObj["v_p1"] = best.v_p1;
    bestObj["peakMotorPower"] = best.peakMotorPower;
    bestObj["travelTime"] = best.travelTime;
    bestObj["fuzzyScore"] = best.fuzzyScore;
    response["best"] = bestObj;
  } else {
    response["best"] = QJsonObject();
  }

  response["totalCombinations"] = m_optimizationHandler->getTotalCombinations();
  response["completedCombinations"] =
      (int)m_optimizationHandler->getResults().size();
  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

// QHttpServerResponse
// ApiHandler::handleStartOptimization(const QJsonObject &data) {
//   QJsonObject response;

//   if (!m_context.trainData || !m_context.massData || !m_context.stationData)
//   {
//     response["status"] = "error";
//     response["message"] = "Context data (Train/Mass/Station) not
//     initialized"; return
//     QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::BadRequest);
//   }

//   if (m_optimizationHandler->isRunning()) {
//     response["status"] = "error";
//     response["message"] = "Optimization already running";
//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Conflict);
//   }

//   // Start optimization
//   m_optimizationHandler->startOptimization(
//       *m_context.trainData, *m_context.massData, *m_context.stationData);

//   response["status"] = "success";
//   response["message"] = "Optimization started";
//   return QHttpServerResponse(QJsonDocument(response).toJson(),
//                              QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleStopOptimization() {
//   m_optimizationHandler->stopOptimization();
//   QJsonObject response;
//   response["status"] = "success";
//   response["message"] = "Optimization stop requested";
//   return QHttpServerResponse(QJsonDocument(response).toJson(),
//                              QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleGetOptimizationStatus() {
//   QJsonObject response;

//   OptimizationResult result = m_optimizationHandler->getResult();

//   response["isRunning"] = m_optimizationHandler->isRunning();
//   response["iteration"] = result.iterationCount;
//   response["suitabilityScore"] = result.suitabilityScore;
//   response["suitabilityLabel"] = result.suitabilityLabel;

//   // Convert score history to array
//   QJsonArray history;
//   for (double score : result.scoreHistory) {
//     history.append(score);
//   }
//   response["scoreHistory"] = history;

//   // Optimized Train Data
//   QJsonObject trainObj;
//   trainObj["n_tm"] = result.optimizedTrain.n_tm;
//   trainObj["gearRatio"] = result.optimizedTrain.gearRatio;
//   // Add other fields if optimized
//   response["optimizedTrain"] = trainObj;

//   // Debug metrics from result
//   response["debug_acc"] = result.debug_acc;
//   response["debug_wp"] = result.debug_wp;
//   response["debug_grad"] = result.debug_grad;
//   response["debug_speed"] = result.debug_speed;

//   return QHttpServerResponse(QJsonDocument(response).toJson(),
//                              QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleApplyOptimization() {
//   QJsonObject response;

//   OptimizationResult result = m_optimizationHandler->getResult();
//   if (result.suitabilityScore <= 0.0) {
//     response["status"] = "error";
//     response["message"] = "No valid optimization result to apply";
//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::BadRequest);
//   }

//   // Apply to context
//   if (m_context.trainData) {
//     m_context.trainData->n_tm = result.optimizedTrain.n_tm;
//     m_context.trainData->gearRatio = result.optimizedTrain.gearRatio;
//     // Apply other optimized fields
//   }

//   response["status"] = "success";
//   response["message"] = "Optimization applied to train parameters";
//   return QHttpServerResponse(QJsonDocument(response).toJson(),
//                              QHttpServerResponse::StatusCode::Ok);
// }