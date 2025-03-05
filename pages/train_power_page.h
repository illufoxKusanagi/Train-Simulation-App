#ifndef TRAIN_POWER_PAGE_H
#define TRAIN_POWER_PAGE_H

#include "../resources/input_type.h"
#include "../widgets/button_action.h"
#include "../widgets/chart_widget.h"
#include "../widgets/input_widget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class TrainPowerPage : public QWidget {
  Q_OBJECT
public:
  explicit TrainPowerPage(QWidget *parentWidget = nullptr);

private:
  QVBoxLayout *mainLayout;
  void setupChart();
  void setupInputs();
};

#endif // TRAIN_POWER_PAGE_H