#include "running_parameter_page.h"

RunningParameterPage::RunningParameterPage(
    AppContext &context, TrainParameterPage *trainParameterPage,
    QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formLayout(new QWidget(this)),
      m_inputsLayout(new QGridLayout(m_formLayout)),
      m_trainParameterPage(trainParameterPage),
      resistanceData(context.resistanceData.data()),
      movingData(context.movingData.data()), loadData(context.loadData.data()),
      trainData(context.trainData.data()) {
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setContentsMargins(16, 16, 16, 16);
  m_inputsLayout->setHorizontalSpacing(64);
  m_inputsLayout->setVerticalSpacing(32);
  createInputs();
  connectInputSignals();
  mainLayout->addWidget(m_formLayout);
  setLayout(mainLayout);
}

void RunningParameterPage::createInputs() {
  QStringList labels = {"Starting Resistance",
                        "Weakening Point 1 (Powering)",
                        "Weakening Point 2 (Powering)",
                        "Difference Coasting Speed",
                        "Weakening Point 3 (Braking)",
                        "Weakening Point 4 (Braking)",
                        "Acceleration",
                        "Powering Gear",
                        "Final Acceleration",
                        "Deceleration",
                        "Decelerating Gear",
                        "Final Deceleration",
                        "Emergency Brake Deceleration"};
  QStringList unitLabels = {"",     "km/h",  "km/h", "km/h",  "km/h",
                            "km/h", "m/s^2", "",     "m/s^2", "m/s^2",
                            "",     "m/s^2", "m/s^2"};
  QStringList poweringOptions = {"P7", "P6", "P5", "P4", "P3", "P2", "P1"};
  QStringList deceleratingOptions = {"B7", "B6", "B5", "B4", "B3", "B2", "B1"};
  QList<double> values = {39.2, 35, 65, 5, 55, 70, 1, 0, 0, 1, 0, 0, 1.2};

  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget = new InputWidget(
        this,
        InputType(i == labels.size() - 3 || i == 7 ? "dropdown" : "field",
                  labels[i], unitLabels[i]),
        (i == labels.size() - 3 || i == 7)
            ? (i == 7 ? poweringOptions : deceleratingOptions)
            : QStringList());
    inputWidget->setValue(values[i]);
    inputWidget->setFixedHeight(80);
    m_inputsLayout->addWidget(inputWidget, i / 3, i % 3);
    m_inputWidgets[labels[i]] = inputWidget;
  }
  setParameterValue();
}

double RunningParameterPage::getParameterValue(const QString &paramName) const {
  if (m_inputWidgets.contains(paramName)) {
    return m_inputWidgets[paramName]->getValue();
  }
  return 0.0;
}

void RunningParameterPage::setParameterValue() {
  resistanceData->startRes = getParameterValue("Starting Resistance");
  movingData->v_p1 = getParameterValue("Weakening Point 1 (Powering)");
  movingData->v_p2 = getParameterValue("Weakening Point 2 (Powering)");
  movingData->v_b1 = getParameterValue("Weakening Point 3 (Braking)");
  movingData->v_diffCoast = getParameterValue("Difference Coasting Speed");
  movingData->v_b2 = getParameterValue("Weakening Point 4 (Braking)");
  movingData->decc_emergency =
      getParameterValue("Emergency Brake Deceleration");
  setAccelerationValue();
  setDecelerationValue();
}

void RunningParameterPage::connectInputSignals() {
  for (auto it = m_inputWidgets.constBegin(); it != m_inputWidgets.constEnd();
       ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setParameterValue();
      double value = getParameterValue(paramName);
    });
  }
  connect(m_trainParameterPage, &TrainParameterPage::trainsetChanged, this,
          [this]() { setAccelerationValue(); });
  connect(m_trainParameterPage, &TrainParameterPage::trainsetChanged, this,
          [this]() { setDecelerationValue(); });
}

void RunningParameterPage::setAccelerationValue() {
  double accelerationIndex = getParameterValue("Powering Gear");
  double originalAcceleration = getParameterValue("Acceleration");
  double motorCarNumber = trainData->n_M1 + trainData->n_M2;
  double newAcceleration = originalAcceleration *
                           ((7 - accelerationIndex) / 7) * (motorCarNumber / 6);
  m_inputWidgets["Final Acceleration"]->setValue(newAcceleration);
  movingData->acc_start = newAcceleration;
}

void RunningParameterPage::setDecelerationValue() {
  double decelerationIndex = getParameterValue("Decelerating Gear");
  double originalDecceleration = getParameterValue("Deceleration");
  double motorCarNumber = trainData->n_M1 + trainData->n_M2;
  double newDecceleration = originalDecceleration *
                            ((7 - decelerationIndex) / 7) *
                            (motorCarNumber / 6);
  m_inputWidgets["Final Deceleration"]->setValue(newDecceleration);
  movingData->decc_start = newDecceleration;
}

void RunningParameterPage::onAwChanged(double awIndex) {
  double newVb1;
  double newVp1;
  int index = static_cast<int>(awIndex);
  newVp1 = (index >= 0 && index < 5) ? (35 + (5 * index)) : 35;
  m_inputWidgets["Weakening Point 1 (Powering)"]->setValue(newVp1);
  setParameterValue();
}