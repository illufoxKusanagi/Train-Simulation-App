#include "train_data_handler.h"

TrainDataHandler::TrainDataHandler(AppContext *context, QObject *parent)
    : QObject(parent), m_loadData(context->loadData.data()),
      m_massData(context->massData.data()),
      m_trainData(context->trainData.data()) {}

void TrainDataHandler::setNumberTractionMotorValue(double numberTractionMotor) {
  if (m_trainData) {
    m_trainData->n_tm = numberTractionMotor;
  }
}

void TrainDataHandler::setNumberAxleValue(double numberAxle) {
  if (m_trainData) {
    m_trainData->n_axle = numberAxle;
  }
}

void TrainDataHandler::setWheelDiameterValue(double wheelDiameter) {
  if (m_trainData) {
    m_trainData->wheel = wheelDiameter;
  }
}

void TrainDataHandler::setGearRatioValue(double gearRatio) {
  if (m_trainData) {
    m_trainData->gearRatio = gearRatio;
  }
}

void TrainDataHandler::setLoadValue(double load) {
  if (m_loadData) {
    m_loadData->load = load;
  }
}

void TrainDataHandler::setPassengerWeightValue(double passengerWeight) {
  if (m_loadData) {
    m_loadData->mass_P = passengerWeight;
  }
}

void TrainDataHandler::setInertialCoefficientTrailerValue(
    double inertialCoefficientTrailer) {
  if (m_massData) {
    m_massData->i_T = inertialCoefficientTrailer;
  }
}

void TrainDataHandler::setInertialCoefficientMotorValue(
    double inertialCoefficientMotor) {
  if (m_massData) {
    m_massData->i_M = inertialCoefficientMotor;
  }
}

void TrainDataHandler::storeFormInputs(
    const QMap<QString, InputWidget *> &inputWidgets) {
  if (inputWidgets.contains("Number of Traction Motor")) {
    setNumberTractionMotorValue(
        inputWidgets["Number of Traction Motor"]->getValue());
  }
  if (inputWidgets.contains("Number of Axle")) {
    setNumberAxleValue(inputWidgets["Number of Axle"]->getValue());
  }
  if (inputWidgets.contains("Wheel Diameter")) {
    setWheelDiameterValue(inputWidgets["Wheel Diameter"]->getValue());
  }
  if (inputWidgets.contains("Gear Ratio")) {
    setGearRatioValue(inputWidgets["Gear Ratio"]->getValue());
  }
  if (inputWidgets.contains("Load per Car")) {
    setLoadValue(inputWidgets["Load per Car"]->getValue());
  }
  if (inputWidgets.contains("Passenger Weight")) {
    setPassengerWeightValue(inputWidgets["Passenger Weight"]->getValue());
  }
  if (inputWidgets.contains("Inertial Coefficient Trailer")) {
    setInertialCoefficientTrailerValue(
        inputWidgets["Inertial Coefficient Trailer"]->getValue());
  }
  if (inputWidgets.contains("Inertial Coefficient Motor")) {
    setInertialCoefficientMotorValue(
        inputWidgets["Inertial Coefficient Motor"]->getValue());
  }
}