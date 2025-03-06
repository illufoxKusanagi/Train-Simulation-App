#ifndef TRAIN_SPEED_PAGE_H
#define TRAIN_SPEED_PAGE_H

#include "../models/train_simulation.h"
#include "../resources/input_type.h"
#include "../widgets/chart_widget.h"
#include "../widgets/input_widget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class TrainSpeedPage : public QWidget {
  Q_OBJECT
public:
  explicit TrainSpeedPage(QWidget *parent = nullptr,
                          TrainSimulation *trainSimulation = nullptr);

private:
  QVBoxLayout *mainLayout;
  TrainSimulation *m_trainSimulation;
  InputWidget *m_inputWidget;
  void setupChart();
  void setupExactValues();
  void setParameterValue();
};

#endif // TRAIN_SPEED_PAGE_H