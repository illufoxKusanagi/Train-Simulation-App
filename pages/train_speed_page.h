#ifndef TRAIN_SPEED_PAGE_H
#define TRAIN_SPEED_PAGE_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class TrainSpeedPage : public QWidget {
  Q_OBJECT
public:
  explicit TrainSpeedPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
};

#endif // TRAIN_SPEED_PAGE_H