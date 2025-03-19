#ifndef TRAIN_SIMULATION_H
#define TRAIN_SIMULATION_H

#define _USE_MATH_DEFINES
#include "train_data.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
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
  explicit TrainSimulation(QObject *parent = nullptr,
                           TrainData *trainData = nullptr,
                           MassData *massData = nullptr,
                           LoadData *loadData = nullptr,
                           ResistanceData *resistanceData = nullptr,
                           MovingData *movingData = nullptr,
                           TrainMotorData *trainMotorData = nullptr,
                           EfficiencyData *efficiencyData = nullptr,
                           PowerData *powerData = nullptr,
                           EnergyData *energyData = nullptr);

  struct SimulationDatas {
    QList<double> trainSpeeds;
    QList<double> tractionEfforts;
    QList<double> vvvfPowers;
    QList<double> catenaryPowers;
    QList<double> vvvfCurrents;
    QList<double> catenaryCurrents;
    QList<double> time;
    QList<double> timeTotal;
    QList<double> distance;
    QList<double> distanceTotal;
    QList<QString> phase;
    QList<double> motorForce;
    QList<double> motorResistance;
    QList<double> totalResistance;
    QList<double> tractionForcePerMotor;
    QList<double> resistancePerMotor;
    QList<double> torque;
    QList<double> rpm;
    QList<double> powerWheel;
    QList<double> powerMotorOut;
    QList<double> powerMotorIn;
    QList<double> powerVvvfIn;
    QList<double> powerCatenary;
    QList<double> currentCatenary;
    QList<double> currentVvvf;
    QList<double> accelerations;
    QList<double> energyConsumptions;
    QList<double> energyPowerings;
    QList<double> energyRegenerations;
    QList<double> energyAps;
  } simulationDatas;

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
  double getAdhesion();

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

  void addOutputDatas(double speed, double tractionEffort, double vvvfPower,
                      double catenaryPower, double vvvfCurrent,
                      double catenaryCurrent);
  void addSimulationDatas(int i, double time, QString phase);
  void clearSimulationDatas();

  void deleteCsvFile(QString csvPath);
  void readCsvFile(const QString path, QStringList &values);

  double calculateEnergyConsumption(int i);
  double calculateEnergyOfPowering(int i);
  double calculateEnergyRegeneration(int i);
  double calculateEnergyOfAps(int i);
};
#endif // TRAIN_SIMULATION_H
