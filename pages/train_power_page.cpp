#include "train_power_page.h"

TrainPowerPage::TrainPowerPage(QWidget *parent,
                               TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setSpacing(40);
  // connect(m_trainSimulation, &TrainSimulation::powerValuesChanged, this,
  //         &TrainPowerPage::onPowerValuesChanged);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainPowerPage::setParameterValue);
  setupInputs();
  ChartWidget *chartWidget =
      new ChartWidget("Train Power", "speed", this, m_trainSimulation);
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
        new InputWidget(InputType("field", labels[i], units[i], 0, true), this);
    // exactValueLayout->addWidget(inputWidget, i / 2, i % 2);
    exactValueLayout->addWidget(inputWidget);
    m_inputWidgets[labels[i]] = inputWidget;
  }
  exactValueLayout->setAlignment(Qt::AlignCenter);
  exactValueLayout->setSpacing(40);
  // connect(m_inputWidgets, &InputWidget::valueChanged, this,
  //         [this] { setParameterValue(); });
  // exactValueLayout->setHorizontalSpacing(120);
  // exactValueLayout->setVerticalSpacing(32);
  mainLayout->addLayout(exactValueLayout);
}

void TrainPowerPage::setParameterValue() {
  m_inputWidgets["Max Catenary Power"]->setValue(0);
  m_inputWidgets["Max VVVF Power"]->setValue(0);
  m_inputWidgets["Max Catenary Current"]->setValue(0);
  m_inputWidgets["Max VVVF Current"]->setValue(0);

  m_inputWidgets["Max Catenary Power"]->setValue(
      m_trainSimulation->findMaxCatenaryPower());
  m_inputWidgets["Max VVVF Power"]->setValue(
      m_trainSimulation->findMaxVvvfPower());
  m_inputWidgets["Max Catenary Current"]->setValue(
      m_trainSimulation->findMaxCatenaryCurrent());
  m_inputWidgets["Max VVVF Current"]->setValue(
      m_trainSimulation->findMaxVvvfCurrent());
}

// void TrainPowerPage::onPowerValuesChanged(double vvvfPower,
//                                           double catenaryPower,
//                                           double vvvfCurrent,
//                                           double catenaryCurrent) {
//   // Only update if greater than current value
//   double currentVvvfPower = m_inputWidgets["Max VVVF Power"]->getValue();
//   if (vvvfPower > currentVvvfPower) {
//     m_inputWidgets["Max VVVF Power"]->setValue(vvvfPower);
//   }

//   double currentCatenaryPower =
//       m_inputWidgets["Max Catenary Power"]->getValue();
//   if (catenaryPower > currentCatenaryPower) {
//     m_inputWidgets["Max Catenary Power"]->setValue(catenaryPower);
//   }

//   double currentVvvfCurrent = m_inputWidgets["Max VVVF Current"]->getValue();
//   if (vvvfCurrent > currentVvvfCurrent) {
//     m_inputWidgets["Max VVVF Current"]->setValue(vvvfCurrent);
//   }

//   double currentCatenaryCurrent =
//       m_inputWidgets["Max Catenary Current"]->getValue();
//   if (catenaryCurrent > currentCatenaryCurrent) {
//     m_inputWidgets["Max Catenary Current"]->setValue(catenaryCurrent);
//   }
// }