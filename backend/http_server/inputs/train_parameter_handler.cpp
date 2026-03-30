#include "train_parameter_handler.h"
#include "controllers/simulation/mass_handler.h"
#include <QJsonDocument>

TrainParameterHandler::TrainParameterHandler(AppContext &context,
                                             QObject *parent)
    : QObject{parent}, m_context(context) {
  m_massHandler = new MassHandler(context);
}

TrainParameterHandler::~TrainParameterHandler() { delete m_massHandler; }

void TrainParameterHandler::recalculateMasses() {
  if (!m_context.massData || !m_context.loadData || !m_context.trainData) {
    qDebug() << "⚠️ Cannot recalculate masses: data not initialized";
    return;
  }

  m_context.massData->mass_totalEmpty = m_massHandler->countMassEmptyCar();

  if (m_context.loadData->load > 0) {
    m_context.massData->mass_totalLoad = m_massHandler->countMassLoadInput();
    m_context.massData->mass_totalInertial =
        m_massHandler->countInertialMassInput();
  } else {
    m_context.massData->mass_totalLoad = m_massHandler->countMassWithLoad();
    m_context.massData->mass_totalInertial = m_massHandler->countInertialMass();
  }

  qDebug() << "✅ Masses recalculated:";
  qDebug() << "   - Empty:" << m_context.massData->mass_totalEmpty << "tons";
  qDebug() << "   - Loaded:" << m_context.massData->mass_totalLoad << "tons";
  qDebug() << "   - Inertial:" << m_context.massData->mass_totalInertial
           << "tons";
}
QHttpServerResponse TrainParameterHandler::handleGetTrainParameters() {
  QJsonObject response;

  if (!m_context.trainData) {
    response["status"] = "error";
    response["message"] = "Train data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  QJsonObject trainParams;
  trainParams["tractionMotors"] = m_context.trainData->n_tm;
  trainParams["axles"] = m_context.trainData->n_axle;
  trainParams["cars"] = m_context.trainData->n_car;
  trainParams["gearRatio"] = m_context.trainData->gearRatio;
  trainParams["wheelDiameter"] = m_context.trainData->wheel;
  trainParams["trainsetLength"] = m_context.trainData->trainsetLength;
  trainParams["motorCars1"] = m_context.trainData->n_M1;
  trainParams["motorCars2"] = m_context.trainData->n_M2;
  trainParams["trailerCabCars"] = m_context.trainData->n_Tc;
  trainParams["trailerCars1"] = m_context.trainData->n_T1;
  trainParams["trailerCars2"] = m_context.trainData->n_T2;
  trainParams["trailerCars3"] = m_context.trainData->n_T3;
  trainParams["motorCars1Disabled"] = m_context.trainData->n_M1_disabled;
  trainParams["motorCars2Disabled"] = m_context.trainData->n_M2_disabled;

  response["trainParameters"] = trainParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse
TrainParameterHandler::handleUpdateTrainParameters(const QJsonObject &data) {
  QJsonObject response;

  if (!m_context.trainData) {
    response["status"] = "error";
    response["message"] = "Train data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    if (data.contains("trainParameters")) {
      QJsonObject trainParams = data["trainParameters"].toObject();
      qDebug() << "📝 Updating train parameters:" << trainParams;

      if (trainParams.contains("tractionMotors") ||
          trainParams.contains("numberOfMotorCars")) {
        double val = trainParams.contains("tractionMotors")
                         ? trainParams["tractionMotors"].toDouble()
                         : trainParams["numberOfMotorCars"].toDouble();
        if (val < 0)
          throw std::invalid_argument("Traction motors cannot be negative");
        m_context.trainData->n_tm = val;
        qDebug() << "Updated n_tm to:" << m_context.trainData->n_tm;
      }
      if (trainParams.contains("axles") ||
          trainParams.contains("numberOfAxles")) {
        double val = trainParams.contains("axles")
                         ? trainParams["axles"].toDouble()
                         : trainParams["numberOfAxles"].toDouble();
        if (val < 0)
          throw std::invalid_argument("Axles cannot be negative");
        m_context.trainData->n_axle = val;
        qDebug() << "Updated n_axle to:" << m_context.trainData->n_axle;
      }
      if (trainParams.contains("cars") ||
          trainParams.contains("numberOfCars")) {
        double val = trainParams.contains("cars")
                         ? trainParams["cars"].toDouble()
                         : trainParams["numberOfCars"].toDouble();
        if (val <= 0)
          throw std::invalid_argument("Number of cars must be positive");
        m_context.trainData->n_car = val;
        qDebug() << "Updated n_car to:" << m_context.trainData->n_car;
      }
      if (trainParams.contains("gearRatio")) {
        m_context.trainData->gearRatio = trainParams["gearRatio"].toDouble();
        qDebug() << "Updated gearRatio to:" << m_context.trainData->gearRatio;
      }
      if (trainParams.contains("wheelDiameter")) {
        m_context.trainData->wheel = trainParams["wheelDiameter"].toDouble();
        qDebug() << "Updated wheel to:" << m_context.trainData->wheel;
      }
      if (trainParams.contains("trainsetLength")) {
        m_context.trainData->trainsetLength =
            trainParams["trainsetLength"].toDouble();
        qDebug() << "Updated trainsetLength to:"
                 << m_context.trainData->trainsetLength;
      }
      if (trainParams.contains("carLength")) {
        const double carLength = trainParams["carLength"].toDouble();
        if (m_context.trainData->n_car > 0) {
          m_context.trainData->trainsetLength =
              carLength * m_context.trainData->n_car;
          qDebug() << "Updated trainsetLength from carLength to:"
                   << m_context.trainData->trainsetLength;
        }
      }

      if (trainParams.contains("numberOfM1Cars")) {
        m_context.trainData->n_M1 = trainParams["numberOfM1Cars"].toDouble();
        qDebug() << "Updated n_M1 to:" << m_context.trainData->n_M1;
      }
      if (trainParams.contains("numberOfM2Cars")) {
        m_context.trainData->n_M2 = trainParams["numberOfM2Cars"].toDouble();
        qDebug() << "Updated n_M2 to:" << m_context.trainData->n_M2;
      }
      if (trainParams.contains("numberOfTcCars")) {
        m_context.trainData->n_Tc = trainParams["numberOfTcCars"].toDouble();
        qDebug() << "Updated n_Tc to:" << m_context.trainData->n_Tc;
      }
      if (trainParams.contains("numberOfT1Cars")) {
        m_context.trainData->n_T1 = trainParams["numberOfT1Cars"].toDouble();
        qDebug() << "Updated n_T1 to:" << m_context.trainData->n_T1;
      }
      if (trainParams.contains("numberOfT2Cars")) {
        m_context.trainData->n_T2 = trainParams["numberOfT2Cars"].toDouble();
        qDebug() << "Updated n_T2 to:" << m_context.trainData->n_T2;
      }
      if (trainParams.contains("numberOfT3Cars")) {
        m_context.trainData->n_T3 = trainParams["numberOfT3Cars"].toDouble();
        qDebug() << "Updated n_T3 to:" << m_context.trainData->n_T3;
      }
      if (trainParams.contains("numberOfM1DisabledCars")) {
        m_context.trainData->n_M1_disabled =
            trainParams["numberOfM1DisabledCars"].toDouble();
        qDebug() << "Updated n_T3 to:" << m_context.trainData->n_M1_disabled;
      }
      if (trainParams.contains("numberOfM2DisabledCars")) {
        m_context.trainData->n_M2_disabled =
            trainParams["numberOfM2DisabledCars"].toDouble();
        qDebug() << "Updated n_T3 to:" << m_context.trainData->n_M2_disabled;
      }

      if (m_context.loadData) {
        if (trainParams.contains("trainLoad") || trainParams.contains("load")) {
          m_context.loadData->load = trainParams.contains("trainLoad")
                                         ? trainParams["trainLoad"].toDouble()
                                         : trainParams["load"].toDouble();
          qDebug() << "Updated load to:" << m_context.loadData->load;
        }
        if (trainParams.contains("mass_P")) {
          m_context.loadData->mass_P = trainParams["mass_P"].toDouble();
          qDebug() << "Updated mass_P to:" << m_context.loadData->mass_P;
        }
      }

      if (m_context.massData) {
        if (trainParams.contains("i_M")) {
          m_context.massData->i_M = trainParams["i_M"].toDouble();
          qDebug() << "Updated i_M to:" << m_context.massData->i_M;
        }
        if (trainParams.contains("i_T")) {
          m_context.massData->i_T = trainParams["i_T"].toDouble();
          qDebug() << "Updated i_T to:" << m_context.massData->i_T;
        }
      }

      recalculateMasses();

      response["status"] = "success";
      response["message"] = "Train parameters updated successfully";
      qDebug() << "✅ Train parameters updated successfully";
    } else {
      qDebug() << "❌ No train parameters provided in request";
      response["status"] = "error";
      response["message"] = "No train parameters provided";
    }
  } catch (const std::exception &e) {
    qDebug() << "💥 Exception in handleUpdateTrainParameters:" << e.what();
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  } catch (...) {
    qDebug() << "💥 Unknown exception in handleUpdateTrainParameters";
    response["status"] = "error";
    response["message"] = "Unknown error updating parameters";
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse TrainParameterHandler::handleGetCarNumberParameters() {
  QJsonObject response;

  if (!m_context.trainData) {
    response["status"] = "error";
    response["message"] = "Train data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  QJsonObject carNumberParams;

  carNumberParams["n_M1"] = m_context.trainData->n_M1;
  carNumberParams["n_M2"] = m_context.trainData->n_M2;
  carNumberParams["n_Tc"] = m_context.trainData->n_Tc;
  carNumberParams["n_T1"] = m_context.trainData->n_T1;
  carNumberParams["n_T2"] = m_context.trainData->n_T2;
  carNumberParams["n_T3"] = m_context.trainData->n_T3;
  carNumberParams["n_M1_disabled"] = m_context.trainData->n_M1_disabled;
  carNumberParams["n_M2_disabled"] = m_context.trainData->n_M2_disabled;

  response["carNumberParameters"] = carNumberParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse TrainParameterHandler::handleUpdateCarNumberParameters(
    const QJsonObject &data) {
  QJsonObject response;

  if (!m_context.trainData) {
    response["status"] = "error";
    response["message"] = "Train data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    QJsonObject carNumberParams = data;
    qDebug() << "📝 Updating car number parameters:" << carNumberParams;

    if (carNumberParams.contains("n_M1")) {
      double val = carNumberParams["n_M1"].toDouble();
      if (val < 0)
        throw std::invalid_argument("n_M1 cannot be negative");
      m_context.trainData->n_M1 = val;
    }
    if (carNumberParams.contains("n_M2")) {
      m_context.trainData->n_M2 = carNumberParams["n_M2"].toDouble();
    }
    if (carNumberParams.contains("n_Tc")) {
      m_context.trainData->n_Tc = carNumberParams["n_Tc"].toDouble();
    }
    if (carNumberParams.contains("n_T1")) {
      m_context.trainData->n_T1 = carNumberParams["n_T1"].toDouble();
    }
    if (carNumberParams.contains("n_T2")) {
      m_context.trainData->n_T2 = carNumberParams["n_T2"].toDouble();
    }
    if (carNumberParams.contains("n_T3")) {
      m_context.trainData->n_T3 = carNumberParams["n_T3"].toDouble();
    }
    if (carNumberParams.contains("n_M1_disabled")) {
      m_context.trainData->n_M1_disabled =
          carNumberParams["n_M1_disabled"].toDouble();
    }
    if (carNumberParams.contains("n_M2_disabled")) {
      m_context.trainData->n_M2_disabled =
          carNumberParams["n_M2_disabled"].toDouble();
    }

    recalculateMasses();

    response["status"] = "success";
    response["message"] = "Car number parameters updated successfully";
    qDebug() << "✅ Car number parameters updated successfully";
  } catch (const std::exception &e) {
    qDebug() << "💥 Exception in handleUpdateCarNumberParameters:" << e.what();
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse TrainParameterHandler::handleGetPassengerParameters() {
  QJsonObject response;

  if (!m_context.loadData) {
    response["status"] = "error";
    response["message"] = "Load data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  QJsonObject passengerParams;

  passengerParams["n_PTc"] = m_context.loadData->n_PTc;
  passengerParams["n_PM1"] = m_context.loadData->n_PM1;
  passengerParams["n_PM2"] = m_context.loadData->n_PM2;
  passengerParams["n_PT1"] = m_context.loadData->n_PT1;
  passengerParams["n_PT2"] = m_context.loadData->n_PT2;
  passengerParams["n_PT3"] = m_context.loadData->n_PT3;

  response["passengerParameters"] = passengerParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse TrainParameterHandler::handleUpdatePassengerParameters(
    const QJsonObject &data) {
  QJsonObject response;

  if (!m_context.loadData) {
    response["status"] = "error";
    response["message"] = "Load data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    QJsonObject passengerParams = data;
    qDebug() << "📝 Updating passenger parameters:" << passengerParams;

    if (passengerParams.contains("n_PTc")) {
      double val = passengerParams["n_PTc"].toDouble();
      if (val < 0)
        throw std::invalid_argument("n_PTc cannot be negative");
      m_context.loadData->n_PTc = val;
    }
    if (passengerParams.contains("n_PM1")) {
      m_context.loadData->n_PM1 = passengerParams["n_PM1"].toDouble();
    }
    if (passengerParams.contains("n_PM2")) {
      m_context.loadData->n_PM2 = passengerParams["n_PM2"].toDouble();
    }
    if (passengerParams.contains("n_PT1")) {
      m_context.loadData->n_PT1 = passengerParams["n_PT1"].toDouble();
    }
    if (passengerParams.contains("n_PT2")) {
      m_context.loadData->n_PT2 = passengerParams["n_PT2"].toDouble();
    }
    if (passengerParams.contains("n_PT3")) {
      m_context.loadData->n_PT3 = passengerParams["n_PT3"].toDouble();
    }

    recalculateMasses();

    response["status"] = "success";
    response["message"] = "Passenger parameters updated successfully";
    qDebug() << "✅ Passenger parameters updated successfully";
  } catch (const std::exception &e) {
    qDebug() << "💥 Exception in handleUpdatePassengerParameters:" << e.what();
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse TrainParameterHandler::handleGetMassParameters() {
  QJsonObject response;

  if (!m_context.massData) {
    response["status"] = "error";
    response["message"] = "Mass data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  QJsonObject massParams;

  massParams["mass_M1"] = m_context.massData->mass_M1;
  massParams["mass_M2"] = m_context.massData->mass_M2;
  massParams["mass_Tc"] = m_context.massData->mass_TC;
  massParams["mass_T1"] = m_context.massData->mass_T1;
  massParams["mass_T2"] = m_context.massData->mass_T2;
  massParams["mass_T3"] = m_context.massData->mass_T3;

  massParams["i_M"] = m_context.massData->i_M;
  massParams["i_T"] = m_context.massData->i_T;

  massParams["totalEmptyMass"] = m_context.massData->mass_totalEmpty;
  massParams["totalLoadMass"] = m_context.massData->mass_totalLoad;
  massParams["totalInertialMass"] = m_context.massData->mass_totalInertial;

  response["massParameters"] = massParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse
TrainParameterHandler::handleUpdateMassParameters(const QJsonObject &data) {
  QJsonObject response;

  if (!m_context.massData) {
    response["status"] = "error";
    response["message"] = "Mass data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    if (data.contains("massParameters")) {
      QJsonObject massParams = data["massParameters"].toObject();
      qDebug() << "📝 Updating mass parameters:" << massParams;

      if (massParams.contains("mass_M1")) {
        double val = massParams["mass_M1"].toDouble();
        if (val < 0)
          throw std::invalid_argument("Mass M1 cannot be negative");
        m_context.massData->mass_M1 = val;
      }
      if (massParams.contains("mass_M2")) {
        m_context.massData->mass_M2 = massParams["mass_M2"].toDouble();
      }
      if (massParams.contains("mass_Tc")) {
        m_context.massData->mass_TC = massParams["mass_Tc"].toDouble();
      }
      if (massParams.contains("mass_T1")) {
        m_context.massData->mass_T1 = massParams["mass_T1"].toDouble();
      }
      if (massParams.contains("mass_T2")) {
        m_context.massData->mass_T2 = massParams["mass_T2"].toDouble();
      }
      if (massParams.contains("mass_T3")) {
        m_context.massData->mass_T3 = massParams["mass_T3"].toDouble();
      }

      if (massParams.contains("i_M")) {
        m_context.massData->i_M = massParams["i_M"].toDouble();
      }
      if (massParams.contains("i_T")) {
        m_context.massData->i_T = massParams["i_T"].toDouble();
      }

      recalculateMasses();

      response["status"] = "success";
      response["message"] = "Mass parameters updated successfully";
      qDebug() << "✅ Mass parameters updated successfully";
    } else {
      response["status"] = "error";
      response["message"] = "No mass parameters provided";
    }
  } catch (const std::exception &e) {
    qDebug() << "💥 Exception in handleUpdateMassParameters:" << e.what();
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse
TrainParameterHandler::handleCalculateMass(const QJsonObject &data) {
  AppContext tempContext;

  if (data.contains("trainset")) {
    QJsonObject trainset = data["trainset"].toObject();
    tempContext.trainData->n_car = trainset["n_car"].toDouble();
    tempContext.trainData->n_M1 = trainset["n_M1"].toDouble();
    tempContext.trainData->n_M2 = trainset["n_M2"].toDouble();
    tempContext.trainData->n_Tc = trainset["n_Tc"].toDouble();
    tempContext.trainData->n_T1 = trainset["n_T1"].toDouble();
    tempContext.trainData->n_T2 = trainset["n_T2"].toDouble();
    tempContext.trainData->n_T3 = trainset["n_T3"].toDouble();
    tempContext.trainData->n_M1_disabled = trainset["n_M1_disabled"].toDouble();
    tempContext.trainData->n_M2_disabled = trainset["n_M2_disabled"].toDouble();

    tempContext.massData->mass_M1 = trainset["mass_M1"].toDouble();
    tempContext.massData->mass_M2 = trainset["mass_M2"].toDouble();
    tempContext.massData->mass_TC = trainset["mass_Tc"].toDouble();
    tempContext.massData->mass_T1 = trainset["mass_T1"].toDouble();
    tempContext.massData->mass_T2 = trainset["mass_T2"].toDouble();
    tempContext.massData->mass_T3 = trainset["mass_T3"].toDouble();

    tempContext.loadData->n_PM1 = trainset["n_PM1"].toDouble();
    tempContext.loadData->n_PM2 = trainset["n_PM2"].toDouble();
    tempContext.loadData->n_PTc = trainset["n_PTc"].toDouble();
    tempContext.loadData->n_PT1 = trainset["n_PT1"].toDouble();
    tempContext.loadData->n_PT2 = trainset["n_PT2"].toDouble();
    tempContext.loadData->n_PT3 = trainset["n_PT3"].toDouble();
  }

  if (data.contains("constant")) {
    QJsonObject constant = data["constant"].toObject();
    tempContext.loadData->load = constant["load"].toDouble();
    tempContext.loadData->mass_P = constant["mass_P"].toDouble();
    tempContext.massData->i_M = constant["i_M"].toDouble();
    tempContext.massData->i_T = constant["i_T"].toDouble();
  }

  MassHandler tempMassHandler(tempContext);

  double totalEmpty = tempMassHandler.countMassEmptyCar();

  double totalLoad = 0.0;
  if (tempContext.loadData->load > 0) {
    totalLoad = tempMassHandler.countMassLoadInput();
  } else {
    totalLoad = tempMassHandler.countMassWithLoad();
  }

  double totalInertial = 0.0;
  if (tempContext.loadData->load > 0) {
    totalInertial = tempMassHandler.countInertialMassInput();
  } else {
    totalInertial = tempMassHandler.countInertialMass();
  }

  QJsonObject response;
  QJsonObject massParams;

  massParams["totalEmptyMass"] = totalEmpty;
  massParams["totalLoadMass"] = totalLoad;
  massParams["totalInertialMass"] = totalInertial;

  response["massParameters"] = massParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}
