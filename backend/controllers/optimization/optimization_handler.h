#ifndef OPTIMIZATION_HANDLER_H
#define OPTIMIZATION_HANDLER_H

#include "controllers/simulation/train_simulation_handler.h"
#include "models/mass_data.h"
#include "models/simulation_data.h"
#include "models/station_data.h"
#include "models/train_data.h"
#include "utils/fuzzy_engine.h"
#include <QAtomicInt>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <memory>
#include <qobject.h>

// Struct to hold optimization results

class OptimizationHandler : public QObject {
  Q_OBJECT

public:
  explicit OptimizationHandler(QObject *parent = nullptr);
  ~OptimizationHandler();

private:
};

#endif // OPTIMIZATION_HANDLER_H
