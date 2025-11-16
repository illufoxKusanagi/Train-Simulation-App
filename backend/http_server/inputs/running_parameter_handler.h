#ifndef RUNNING_PARAMETER_HANDLER_H
#define RUNNING_PARAMETER_HANDLER_H

#include <QObject>
#include <qhttpserverresponse.h>
#include <qjsonobject.h>
#include <core/appcontext.h>

class RunningParameterHandler : public QObject
{
    Q_OBJECT
public:
    explicit RunningParameterHandler(AppContext &context, QObject *parent = nullptr);
    QHttpServerResponse handleGetRunningParameters();
    QHttpServerResponse handleUpdateRunningParameters(const QJsonObject &data);

private:
    AppContext &m_context;
};

#endif // RUNNING_PARAMETER_HANDLER_H
