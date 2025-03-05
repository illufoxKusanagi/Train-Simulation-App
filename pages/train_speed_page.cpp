#include "train_speed_page.h"

TrainSpeedPage::TrainSpeedPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setSpacing(40);
  ChartWidget *chartWidget = new ChartWidget("Train Speed", "speed", this);
  setupExactValues();
  mainLayout->addWidget(chartWidget);
  setLayout(mainLayout);
}

void TrainSpeedPage::setupChart() {}

void TrainSpeedPage::setupExactValues() {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", "Max Speed", "km/h");
  InputWidget *inputWidget = new InputWidget(inputType, this);
  layout->addWidget(inputWidget);
  mainLayout->addLayout(layout);
}