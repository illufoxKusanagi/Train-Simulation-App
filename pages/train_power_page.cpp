#include "train_power_page.h"

TrainPowerPage::TrainPowerPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setSpacing(40);
  setupInputs();
  ChartWidget *chartWidget = new ChartWidget("Train Power", "speed", this);
  mainLayout->addWidget(chartWidget);
  setLayout(mainLayout);
}

void TrainPowerPage::setupChart() {}

void TrainPowerPage::setupInputs() {
  // QGridLayout *exactValueLayout = new QGridLayout(this);
  QHBoxLayout *exactValueLayout = new QHBoxLayout;
  QStringList labels = {"Max Catenary Power", "Max VVVF Power",
                        "Max Catenary Current", "Max VVVF Current"};
  QStringList units = {"kN", "kN", "Ampere", "Ampere"};
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], units[i]), this);
    // exactValueLayout->addWidget(inputWidget, i / 2, i % 2);
    exactValueLayout->addWidget(inputWidget);
  }
  exactValueLayout->setAlignment(Qt::AlignCenter);
  exactValueLayout->setSpacing(40);
  // exactValueLayout->setHorizontalSpacing(120);
  // exactValueLayout->setVerticalSpacing(32);
  mainLayout->addLayout(exactValueLayout);
}