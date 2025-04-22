#ifndef SAVE_BUTTON_HANDLER_H
#define SAVE_BUTTON_HANDLER_H

#include "controllers/train_simulation/train_simulation.h"
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
  explicit SaveButtonHandler(TrainSimulation *trainSimulation,
                             QString &chartTitle, QString *type);
  TrainSimulation *m_trainSimulation;
  QString m_chartTitle;
  QString m_simulationType = "";
};

#endif // SAVE_BUTTON_HANDLER_H
