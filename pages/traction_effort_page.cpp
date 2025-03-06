#include "traction_effort_page.h"

TractionEffortPage::TractionEffortPage(QWidget *parent,
                                       TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setSpacing(40);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TractionEffortPage::setParameterValue);
  setupExactValue();
  ChartWidget *chartWidget =
      new ChartWidget("Traction Effort", "speed", this, m_trainSimulation);
  mainLayout->addWidget(chartWidget);
  setLayout(mainLayout);
}

void TractionEffortPage::setupChart() {
  // m_stackedWidget->addWidget(chartWidget);
}

void TractionEffortPage::setupExactValue() {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", "Max Traction Effort", "kN");
  m_inputWidget = new InputWidget(inputType, this);
  layout->addWidget(m_inputWidget);
  mainLayout->addLayout(layout);
  // m_stackedWidget->addWidget(inputWidget);
}

void TractionEffortPage::setParameterValue() {
  m_inputWidget->setValue(0);
  m_inputWidget->setValue(m_trainSimulation->findMaxTractionEffort());
}