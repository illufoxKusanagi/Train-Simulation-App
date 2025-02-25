#ifndef TRAIN_SIMULATION_H
#define TRAIN_SIMULATION_H

#define _USE_MATH_DEFINES
#include "train_data.h"
#include <QString>
#include <cmath>

ConstantData constantData;
TrainData trainData;
MassData massData;
LoadData loadData;
ResistanceData resistanceData;
MovingData movingData;
TrainMotorData trainMotorData;
EfficiencyData efficiencyData;
PowerData powerData;
EnergyData energyData;

double countMassEmptyCar();
double countMassWithLoad();
double countMassLoadInput();
double countInertialMass();
double countInertialMassInput();
double calculateResTrain(float m, float startRes);
double calculateResSlope(float m, float slope);
double calculateResRadius(float m, float radius);
double calculateStartForce();
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
void calculatePoweringForce(float acc, float v);
void calculateBrakingForce();
void simulateStaticTrainMovement();
void simulateDynamicTrainMovement();

double calculateEnergyConsumption();
double calculateEnergyOfPowering();
double calculateEnergyRegeneration();
double calculateEnergyOfAps();

#endif // TRAIN_SIMULATION_H
