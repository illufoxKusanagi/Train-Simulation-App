#ifndef ELECTRICAL_PARAMETER_HANDLER_H
#define ELECTRICAL_PARAMETER_HANDLER_H

#include <QJsonDocument>
#include <QObject>
#include <core/appcontext.h>
#include <qhttpserverresponse.h>
#include <qjsonobject.h>

class ElectricalParameterHandler : public QObject {
  Q_OBJECT
public:
  explicit ElectricalParameterHandler(AppContext &context,
                                      QObject *parent = nullptr);
  QHttpServerResponse handleGetElectricalParameters();
  QHttpServerResponse handleUpdateElectricalParameters(const QJsonObject &data);

private:
  AppContext &m_context;
};

#endif // ELECTRICAL_PARAMETER_HANDLER_H
