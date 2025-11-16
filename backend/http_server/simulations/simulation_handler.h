#ifndef SIMULATION_HANDLER_H
#define SIMULATION_HANDLER_H

#include <QObject>

class SimulationHandler : public QObject
{
    Q_OBJECT
public:
    explicit SimulationHandler(QObject *parent = nullptr);

signals:
};

#endif // SIMULATION_HANDLER_H
