#ifndef TRAIN_SIMULATION_H
#define TRAIN_SIMULATION_H

#define _USE_MATH_DEFINES
#include "core/appcontext.h"
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
  void resetSimulation();

public:
  explicit TrainSimulation(AppContext &context, QObject *parent = nullptr);
  QSharedPointer<SimulationDatas> simulationDatas;

  double countMassEmptyCar();
  double countMassWithLoad();
  double countMassLoadInput();
  double countInertialMass();
  double countInertialMassInput();

  bool saveTrainSpeedData();
  bool saveTractionEffortData();
  bool saveTrainPowerData();
  bool saveTrainTrackData();
  bool saveEnergyConsumptionData();
  bool saveEnergyPoweringData();
  bool saveEnergyRegenData();
  bool saveEnergyApsData();
  void printSimulationDatas();

  double findMaxSpeed();
  double findMaxVvvfPower();
  double findMaxCatenaryPower();
  double findMaxVvvfCurrent();
  double findMaxCatenaryCurrent();
  double findMaxTractionEffort();
  double findDistanceTravelled();
  double findMaxEnergyConsumption();
  double findMaxEnergyRegen();
  double findMaxEnergyPowering();
  double findMaxEnergyAps();
  double findMaxCurrTime();
  double findMaxPowTime();
  double getAdhesion();

  double calculateBrakingTrack();
  double calculateNormalSimulationTrack();
  double calculateDelaySimulationTrack();
  double calculateSafetySimulationTrack();
  double calculateBrakingEmergencyTrack();
  double calculateEmergencyNormalSimulationTrack();
  double calculateEmergencyDelaySimulationTrack();
  double calculateEmergencySafetySimulationTrack();

  int getAllDataNumber();
  int getTrainSpeedDataNumber();
  int getTractionEffortDataNumber();
  int getTrainPowerDataNumber();
  int getTrainTrackDataNumber();
  int getEnergyConsumptionDataNumber();

  bool validateCsvVariables();

private:
  ConstantData constantData;
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
  double calculateResTrain(float m, float startRes);
  double calculateResSlope(float m, float slope);
  double calculateResRadius(float m, float radius);
  double calculateStartForce(float acc);
  double calculateTotalForce(float v);
  double calculateBrakeForce();
  double calculateTotalBrakeForce();
  double calculateTractionForce();
  double calculateTorque();
  double calculateRpm();
  double calculateAdhesion();
  double calculateResistanceForcePerMotor(double f_res);
  double calculatePowerWheel();
  double calculateOutputTractionMotor();
  double calculateInputTractionMotor();
  double calculatePowerInputOfVvvf();
  double calculatePowerOfCatenary();
  double calculateStartRes();
  double calculateRunningRes(float v);
  void calculatePoweringForce(float acc, float v);
  void calculateBrakingForce();
  double calculateVvvfCurrent();
  double calculateCatenaryCurrent();
  double calculateTotalTime(int i);
  double calculateTotalDistance(int i);
  double calculateEnergyConsumption(int i);
  double calculateEnergyOfPowering(int i);
  double calculateEnergyRegeneration(int i);
  double calculateEnergyOfAps(int i);
  double calculateMileage();

  void addOutputDatas(double speed, double tractionEffort, double vvvfPower,
                      double catenaryPower, double vvvfCurrent,
                      double catenaryCurrent);
  void addSimulationDatas(int i, double time, QString phase);
  void clearSimulationDatas();

  void deleteCsvFile(QString csvPath);
  void readCsvFile(const QString path, QStringList &values);
};
#endif // TRAIN_SIMULATION_H
