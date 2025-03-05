#ifndef TRAIN_SPEED_PAGE_H
#define TRAIN_SPEED_PAGE_H

#include "../resources/input_type.h"
#include "../widgets/chart_widget.h"
#include "../widgets/input_widget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>


class TrainSpeedPage : public QWidget {
  Q_OBJECT
public:
  explicit TrainSpeedPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  void setupChart();
  void setupExactValues();
};

#endif // TRAIN_SPEED_PAGE_H