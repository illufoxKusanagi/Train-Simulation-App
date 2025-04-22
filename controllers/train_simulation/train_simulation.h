#ifndef TRAIN_SIMULATION_H
#define TRAIN_SIMULATION_H

#define _USE_MATH_DEFINES
#include "core/appcontext.h"
#include "current_handler.h"
#include "energy_handler.h"
#include "mass_handler.h"
#include "moving_handler.h"
#include "output_handler.h"
#include "power_handler.h"
#include "resistance_handler.h"
#include "simulation_track_handler.h"
#include "traction_motor_handler.h"
#include "tractive_effort_handler.h"
#include "utility_handler.h"
#include "widgets/message_box_widget.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <cmath>
#include <fstream>
#include <iostream>
#include <qdebug.h>

class TrainSimulation : public QObject {
  Q_OBJECT

signals:
  void simulationCompleted();
  void staticSimulationCompleted();
  void powerValuesChanged(double vvvfPower, double catenaryPower,
                          double vvvfCurrent, double catenaryCurrent);

public slots:
  void simulateStaticTrainMovement();
  void simulateDynamicTrainMovement();

public:
  explicit TrainSimulation(AppContext &context, QObject *parent = nullptr);
  SimulationDatas &simulationDatas;
  MassHandler *m_massHandler;
  EnergyHandler *m_energyHandler;
  PowerHandler *m_powerHandler;
  ResistanceHandler *m_resistanceHandler;
  SimulationTrackHandler *m_simulationTrackHandler;
  TractionMotorHandler *m_tractionMotorHandler;
  TractiveEffortHandler *m_tractiveEffortHandler;
  UtilityHandler *m_utilityHandler;
  OutputHandler *m_outputHandler;
  CurrentHandler *m_currentHandler;

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

  bool validateCsvVariables();

private:
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
  double m_maxVvvfCurrent = 0;
  double m_maxVvvfPower = 0;

  void initTrainMassData();
  void initData();
  double calculateTotalTime(int i);
  double calculateTotalDistance(int i);
  void calculatePowers();
  void calculateEnergies(int i);
  int setSlopeIndex(int slopeIndex, double distanceTravelled);
  int setRadiusIndex(int radiusIndex, double distanceTravelled);
  int setMaxSpeedIndex(int maxSpeedIndex, double distanceTravelled);
  double setSlopeData(int slopeIndex, double distanceTravelled);
  double setRadiusData(int radiusIndex, double distanceTravelled);
  double setMaxSpeedData(int maxSpeedIndex, double distanceTravelled);
};
#endif // TRAIN_SIMULATION_H
