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

void TrainDataHandler::setNumberCarValue(double numberCar) {
  if (m_trainData) {
    m_trainData->n_car = numberCar;
  }
}

void TrainDataHandler::setTrainsetLengthValue(double carLength) {
  if (m_trainData) {
    m_trainData->trainsetLength = carLength * m_trainData->n_car;
  }
}

void TrainDataHandler::setDegradedTrainMotorValues(double numberM1,
                                                   double numberM2) {
  if (m_trainData) {
    m_trainData->n_M1_disabled = numberM1;
    m_trainData->n_M2_disabled = numberM2;
  }
}