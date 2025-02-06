#ifndef TRAIN_POWER_PAGE_H
#define TRAIN_POWER_PAGE_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>


class TrainPowerPage : public QWidget {
  Q_OBJECT
public:
  explicit TrainPowerPage(QWidget *parentWidget = nullptr);

private:
  QVBoxLayout *mainLayout;
};

#endif // TRAIN_POWER_PAGE_H
