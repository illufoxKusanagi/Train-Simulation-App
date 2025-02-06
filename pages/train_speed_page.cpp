#include "train_speed_page.h"

TrainSpeedPage::TrainSpeedPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  setLayout(mainLayout);
  QLabel *label = new QLabel("Train Speed Page", this);
}