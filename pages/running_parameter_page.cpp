#include "running_parameter_page.h"

RunningParameterPage::RunningParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formLayout(new QWidget(this)),
      m_inputsLayout(new QGridLayout(m_formLayout)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setContentsMargins(16, 16, 16, 16);
  m_inputsLayout->setHorizontalSpacing(80);
  m_inputsLayout->setVerticalSpacing(24);

  createInputs();
  connectInputSignals();
  mainLayout->addWidget(m_formLayout);
  setLayout(mainLayout);
}

void RunningParameterPage::createInputs() {
  QStringList labels = {
      "Starting Resistance",
      "Weakening Point 1 (Powering)",
      "Acceleration",
      "Weakening Point 2 (Powering)",
      "Deceleration",
      "Weakening Point 3 (Braking)",
      "Difference Coasting Speed",
      "Weakening Point 4 (Braking)",
  };
  QStringList unitLabels = {"",      "km/h", "m/s^2", "km/h",
                            "m/s^2", "km/h", "km/h",  "km/h"};
  QList<double> values = {39.2, 35, 1, 65, 1, 1, 5, 70};

  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], unitLabels[i]), this);
    inputWidget->setValue(values[i]);
    m_inputsLayout->addWidget(inputWidget, i / 2, i % 2);
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
  resistanceData.startRes = getParameterValue("Starting Resistance");
  movingData.v_p1 = getParameterValue("Weakening Point 1 (Powering)");
  movingData.acc_start = getParameterValue("Acceleration");
  movingData.v_p2 = getParameterValue("Weakening Point 2 (Powering)");
  movingData.decc_start = getParameterValue("Deceleration");
  movingData.v_b1 = getParameterValue("Weakening Point 3 (Braking)");
  movingData.v_diffCoast = getParameterValue("Difference Coasting Speed");
  movingData.v_b2 = getParameterValue("Weakening Point 4 (Braking)");
}

void RunningParameterPage::connectInputSignals() {
  for (auto it = m_inputWidgets.constBegin(); it != m_inputWidgets.constEnd();
       ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setParameterValue();
      double value = getParameterValue(paramName);
      qDebug() << "Parameter" << paramName << "changed to:" << value;

      // Additional debug information
      qDebug() << "Current data values:";
      qDebug() << "  Starting Resistance:" << resistanceData.startRes;
      qDebug() << "  v_p1:" << movingData.v_p1;
      qDebug() << "  acc_start:" << movingData.acc_start;
      qDebug() << "  v_p2:" << movingData.v_p2;
      qDebug() << "  decc_start:" << movingData.decc_start;
      qDebug() << "  v_b1:" << movingData.v_b1;
      qDebug() << "  v_diffCoast:" << movingData.v_diffCoast;
      qDebug() << "  v_b2:" << movingData.v_b2;
    });
  }
}
