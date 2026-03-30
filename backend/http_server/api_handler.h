#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "auth/auth_manager.h"
#include "auth/user_manager.h"
#include "controllers/data/electrical_data_handler.h"
#include "controllers/data/running_data_handler.h"
#include "controllers/data/track_data_handler.h"
#include "controllers/data/train_data_handler.h"
#include "controllers/optimization/optimization_handler.h"
#include "controllers/output/csv_output_handler.h"
#include "core/appcontext.h"
#include "inputs/electrical_parameter_handler.h"
#include "inputs/running_parameter_handler.h"
#include "inputs/track_parameter_handler.h"
#include "inputs/train_parameter_handler.h"
#include "simulations/simulation_handler.h"
#include <QFuture>
#include <QHttpServerResponse>
#include <QJsonObject>
#include <QObject>

class ApiHandler : public QObject {
  Q_OBJECT

public:
  explicit ApiHandler(AppContext &context, QObject *parent = nullptr);
  ~ApiHandler();

  QHttpServerResponse handleHealthCheck();

  QHttpServerResponse handleGetTrainParameters();
  QHttpServerResponse handleUpdateTrainParameters(const QJsonObject &data);

  QHttpServerResponse handleGetElectricalParameters();
  QHttpServerResponse handleUpdateElectricalParameters(const QJsonObject &data);

  QHttpServerResponse handleGetRunningParameters();
  QHttpServerResponse handleUpdateRunningParameters(const QJsonObject &data);

  QHttpServerResponse handleGetTrackParameters();
  QHttpServerResponse handleUpdateTrackParameters(const QJsonObject &data);

  QHttpServerResponse handleGetMassParameters();
  QHttpServerResponse handleUpdateMassParameters(const QJsonObject &data);
  QHttpServerResponse handleCalculateMass(const QJsonObject &data);

  QHttpServerResponse handleGetCarNumberParameters();
  QHttpServerResponse handleUpdateCarNumberParameters(const QJsonObject &data);

  QHttpServerResponse handleGetPassengerParameters();
  QHttpServerResponse handleUpdatePassengerParameters(const QJsonObject &data);

  QHttpServerResponse handleStartSimulation(const QJsonObject &data);
  QHttpServerResponse handleGetSimulationStatus();
  QHttpServerResponse handleGetSimulationResults();

  QHttpServerResponse handleStartOptimization(const QJsonObject &data);
  QHttpServerResponse handleGetOptimizationStatus();

  QHttpServerResponse handleExportResults(const QJsonObject &data);

  QHttpServerResponse handleQuickInit();

  QHttpServerResponse handleLogin(const QJsonObject &data);
  QHttpServerResponse handleGetAuthStatus();

  QHttpServerResponse handleDebugContext();

private:
  AppContext &m_context;

  UserManager *m_userManager;
  AuthManager *m_authManager;

  TrainParameterHandler *m_trainHandler;
  ElectricalParameterHandler *m_electricalHandler;
  RunningParameterHandler *m_runningHandler;
  TrackParameterHandler *m_trackHandler;
  SimulationHandler *m_simulationHandler;
  OptimizationHandler *m_optimizationHandler;
  QFuture<void> m_optimizationFuture;
  TrainDataHandler *m_trainDataHandler;
  ElectricalDataHandler *m_electricalDataHandler;
  RunningDataHandler *m_runningDataHandler;
  TrackDataHandler *m_trackDataHandler;
  CsvOutputHandler *m_csvOutputHandler;
};

#endif // API_HANDLER_H
