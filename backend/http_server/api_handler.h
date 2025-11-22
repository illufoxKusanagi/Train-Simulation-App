#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "controllers/data/electrical_data_handler.h"
#include "controllers/data/running_data_handler.h"
#include "controllers/data/track_data_handler.h"
#include "controllers/data/train_data_handler.h"
#include "controllers/output/csv_output_handler.h"
#include "core/appcontext.h"
#include "inputs/electrical_parameter_handler.h"
#include "inputs/running_parameter_handler.h"
#include "inputs/track_parameter_handler.h"
#include "inputs/train_parameter_handler.h"
#include "simulations/simulation_handler.h"
#include <QHttpServerResponse>
#include <QJsonObject>
#include <QObject>

class ApiHandler : public QObject {
  Q_OBJECT

public:
  explicit ApiHandler(AppContext &context, QObject *parent = nullptr);

  // Health check
  QHttpServerResponse handleHealthCheck();

  // Train parameters
  QHttpServerResponse handleGetTrainParameters();
  QHttpServerResponse handleUpdateTrainParameters(const QJsonObject &data);

  // Electrical parameters
  QHttpServerResponse handleGetElectricalParameters();
  QHttpServerResponse handleUpdateElectricalParameters(const QJsonObject &data);

  // Running parameters
  QHttpServerResponse handleGetRunningParameters();
  QHttpServerResponse handleUpdateRunningParameters(const QJsonObject &data);

  // Track parameters
  QHttpServerResponse handleGetTrackParameters();
  QHttpServerResponse handleUpdateTrackParameters(const QJsonObject &data);

  // Mass parameters
  QHttpServerResponse handleGetMassParameters();
  QHttpServerResponse handleUpdateMassParameters(const QJsonObject &data);
  QHttpServerResponse handleCalculateMass(const QJsonObject &data);

  // Car number parameters
  QHttpServerResponse handleGetCarNumberParameters();
  QHttpServerResponse handleUpdateCarNumberParameters(const QJsonObject &data);

  // Passenger parameters
  QHttpServerResponse handleGetPassengerParameters();
  QHttpServerResponse handleUpdatePassengerParameters(const QJsonObject &data);

  // Simulation control
  QHttpServerResponse handleStartSimulation(const QJsonObject &data);
  QHttpServerResponse handleGetSimulationStatus();
  QHttpServerResponse handleGetSimulationResults();

  // Export functionality
  QHttpServerResponse handleExportResults(const QJsonObject &data);

  // Simple initialization with minimal valid data
  QHttpServerResponse handleQuickInit();

  // Debug endpoint to check current AppContext values
  QHttpServerResponse handleDebugContext();

private:
  AppContext &m_context;

  // New separated sub handler
  TrainParameterHandler *m_trainHandler;
  ElectricalParameterHandler *m_electricalHandler;
  RunningParameterHandler *m_runningHandler;
  TrackParameterHandler *m_trackHandler;
  // MassParameterHandler *m_massHandler;
  // CarNumberHandler *m_carNumberHandler;
  // PassengerHandler *m_passengerHandler;
  SimulationHandler *m_simulationHandler;
  // ExportHandler *m_exportHandler;
  // Edited here: Fixed handler member variable types to match actual classes
  TrainDataHandler *m_trainDataHandler;
  ElectricalDataHandler *m_electricalDataHandler;
  RunningDataHandler *m_runningDataHandler;
  TrackDataHandler *m_trackDataHandler;
  // TrainSimulationHandler *m_simulationHandler;
  CsvOutputHandler *m_csvOutputHandler;
};

#endif // API_HANDLER_H
