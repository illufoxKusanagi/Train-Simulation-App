#ifndef SAVE_BUTTON_HANDLER_H
#define SAVE_BUTTON_HANDLER_H

#include "controllers/simulation/train_simulation_handler.h"
#include "widgets/message_box_widget.h"
#include <QChartView>
#include <QObject>

class SaveButtonHandler : public QObject {
  Q_OBJECT
public slots:
  void onSaveAllDataClicked();
  void onSaveButtonClicked(QChartView *chartView);
  void onSaveCurrentDataClicked();

public:
  enum SimulationType { None, Dynamic, Static };
  explicit SaveButtonHandler(TrainSimulationHandler *trainSimulation,
                             QString &chartTitle, SimulationType *type);
  TrainSimulationHandler *m_trainSimulation;
  QString m_chartTitle;
  SimulationType *m_simulationType;
};

#endif // SAVE_BUTTON_HANDLER_H
