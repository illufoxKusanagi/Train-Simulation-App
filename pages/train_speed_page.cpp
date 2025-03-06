#include "train_speed_page.h"

TrainSpeedPage::TrainSpeedPage(QWidget *parent,
                               TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setSpacing(40);
  ChartWidget *chartWidget =
      new ChartWidget("Train Speed", "speed", this, m_trainSimulation);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainSpeedPage::setParameterValue);
  setupExactValues();
  mainLayout->addWidget(chartWidget);
  setLayout(mainLayout);
}

void TrainSpeedPage::setupChart() {}

void TrainSpeedPage::setupExactValues() {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", "Max Speed", "km/h");
  m_inputWidget = new InputWidget(inputType, this);
  layout->addWidget(m_inputWidget);
  mainLayout->addLayout(layout);
}

void TrainSpeedPage::setParameterValue() {
  m_inputWidget->setValue(0);
  m_inputWidget->setValue(m_trainSimulation->findMaxSpeed());
}