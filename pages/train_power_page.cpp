#include "train_power_page.h"

TrainPowerPage::TrainPowerPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  setLayout(mainLayout);
  QLabel *label = new QLabel("Train Power Page", this);
}