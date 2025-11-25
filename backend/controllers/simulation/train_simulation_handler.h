#ifndef TRAIN_SIMULATION_HANDLER_H
#define TRAIN_SIMULATION_HANDLER_H

#include <qcontainerfwd.h>
#define _USE_MATH_DEFINES
#include "controllers/output/csv_output_handler.h"
#include "core/appcontext.h"
#include "csv_variables_handler.h"
#include "current_handler.h"
#include "energy_handler.h"
#include "mass_handler.h"
#include "power_handler.h"
#include "resistance_handler.h"
#include "simulation_track_handler.h"
#include "traction_motor_handler.h"
#include "tractive_effort_handler.h"
#include "utility_handler.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFuture>
#include <QMutex>
#include <QObject>
#include <QSet>
#include <QString>
#include <QtConcurrent>
#include <qdebug.h>

class TrainSimulationHandler : public QObject {
  Q_OBJECT

signals:
  void simulationCompleted();
  void staticSimulationCompleted();
  void powerValuesChanged(double vvvfPower, double catenaryPower,
                          double vvvfCurrent, double catenaryCurrent);
  void simulationStarted();
  void staticSimulationStarted();
  void simulationError();

public slots:
  void simulateStaticTrainMovement();
  void simulateDynamicTrainMovement();

public:
  explicit TrainSimulationHandler(AppContext &context,
                                  QObject *parent = nullptr);
  SimulationDatas &simulationDatas;
  MassHandler *m_massHandler;
  EnergyHandler *m_energyHandler;
  PowerHandler *m_powerHandler;
  ResistanceHandler *m_resistanceHandler;
  SimulationTrackHandler *m_simulationTrackHandler;
  TractionMotorHandler *m_tractionMotorHandler;
  TractiveEffortHandler *m_tractiveEffortHandler;
  UtilityHandler *m_utilityHandler;
  CsvOutputHandler *m_outputHandler;
  CurrentHandler *m_currentHandler;
  CsvVariableHandler *m_csvVariableHandler;

  double getMaxSpeed();
  double getMaxVvvfPower();
  double getMaxCatenaryPower();
  double getMaxVvvfCurrent();
  double getMaxCatenaryCurrent();
  double getMaxTractionEffort();
  double getDistanceTravelled();
  double getMaxEnergyConsumption();
  double getMaxEnergyRegen();
  double getMaxEnergyPowering();
  double getMaxEnergyAps();
  double getMaxCurrTime();
  double getMaxPowTime();
  double getAdhesion();

  bool validateDataInitialized();
  bool validateCsvVariables();
  void clearWarnings() { m_simulationWarnings->clear(); }
  QStringList getSimulationWarnings() const { return *m_simulationWarnings; }
  void clearErrors() { m_simulationErrors->clear(); }
  QStringList getSimulationErrors() const { return *m_simulationErrors; }
  bool isSimulationRunning() const { return m_simulationFuture.isRunning(); }

private:
  enum Notch { AtStation, Accelerating, Coasting, Braking, None };
  ConstantData *constantData;
  TrainData *trainData;
  MassData *massData;
  LoadData *loadData;
  ResistanceData *resistanceData;
  MovingData *movingData;
  TrainMotorData *trainMotorData;
  EfficiencyData *efficiencyData;
  PowerData *powerData;
  EnergyData *energyData;
  StationData *stationData;
  QStringList *m_simulationWarnings;
  QStringList *m_simulationErrors;
  // QSet<QString> m_simulationWarnings;
  // QSet<QString> m_simulationErrors;

  int m_slopeIndex = 0;
  int m_radiusIndex = 0;
  int m_maxSpeedIndex = 0;
  int m_effVvvfIndex = 0;
  int m_effGearIndex = 0;
  int m_effMotorIndex = 0;
  int m_lineVoltageIndex = 0;
  int m_motorVoltageIndex = 0;

  double m_slope;
  double m_radius;
  double m_maxSpeed;
  double m_lineVoltage;
  double m_motorVoltage;
  double m_efficiencyVvvf;
  double m_efficiencyGear;
  double m_efficiencyMotor;

  void initTrainMassData();
  void initData();
  double calculateTotalTime(int i);
  double calculateTotalDistance(int i);
  void calculatePowers(double efficiencyGear, double efficiencyMotor,
                       double efficiencyVvvf);
  void calculateEnergies(int i);
  int setSlopeIndex(int slopeIndex, double distanceTravelled);
  int setRadiusIndex(int radiusIndex, double distanceTravelled);
  int setMaxSpeedIndex(int maxSpeedIndex, double distanceTravelled);
  int setEffGearIndex(int effGearIndex, double speed);
  int setEffVvvfIndex(int effVvvfIndex, double speed);
  int setEffMotorIndex(int effMotorIndex, double speed);
  int setLineVoltageIndex(int lineVoltageIndex, double speed);
  int setMotorVoltageIndex(int motorVoltageIndex, double speed);
  double setSlopeData(int slopeIndex, double distanceTravelled);
  double setRadiusData(int radiusIndex, double distanceTravelled);
  double setMaxSpeedData(int maxSpeedIndex, double distanceTravelled);
  double setEffGearData(int effGearIndex, double speed);
  double setEffVvvfData(int effVvvfIndex, double speed);
  double setEffMotorData(int effMotorIndex, double speed);
  double setLineVoltageData(int lineVoltageIndex, double speed);
  double setMotorVoltageData(int motorVoltageIndex, double speed);
  void calculateRunningResEachSlope();
  void setCsvVariablesData();
  void addEnergySimulationDatas();
  void addStationSimulationDatas();
  void runDynamicSimulation();
  void runStaticSimulation();
  QMutex *m_simulationMutex;
  QFuture<void> m_simulationFuture;
};
#endif // TRAIN_SIMULATION_HANDLER_H
