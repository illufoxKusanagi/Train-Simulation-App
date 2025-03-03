#include "electrical_parameter_page.h"

ElectricalParameterPage::ElectricalParameterPage(QWidget *parent,
                                                 EfficiencyData *efficiencyData,
                                                 PowerData *powerData)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formContainer(new QWidget(this)),
      m_formLayout(new QGridLayout(m_formContainer)),
      efficiencyData(efficiencyData), powerData(powerData) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setHorizontalSpacing(80);
  m_formLayout->setVerticalSpacing(32);
  createInputs();
  mainLayout->addWidget(m_formContainer);
  setLayout(mainLayout);
}

void ElectricalParameterPage::createInputs() {
  QStringList labels = {"Line Voltage",
                        "Power Factor of Traction Motor",
                        "Motor Voltage",
                        "Efficiency of Gearbox",
                        "Efficiency of VVVF",
                        "Auxiliary Power",
                        "Efficiency of Traction Motor"};
  QStringList units = {"V", "%", "V", "%", "%", "kW", "%"};
  QList<double> values = {1500, 0, 0, 0.98, 0.96, 30, 0.89};
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], ""), this);
    inputWidget->setValue(values[i]);
    m_inputWidgets[labels[i]] = inputWidget;
    m_formLayout->addWidget(inputWidget, i / 2, i % 2);
  }
  setParameterValue();
  connectInputSignals();
}

double
ElectricalParameterPage::getParameterValue(const QString &paramName) const {
  if (m_inputWidgets.contains(paramName)) {
    return m_inputWidgets[paramName]->getValue();
  }
  return 0.0;
}

void ElectricalParameterPage::setParameterValue() {
  efficiencyData->eff_gear = getParameterValue("Efficiency of Gearbox");
  efficiencyData->eff_motor = getParameterValue("Efficiency of Traction Motor");
  efficiencyData->eff_vvvf = getParameterValue("Efficiency of VVVF");
  powerData->p_aps = getParameterValue("Auxiliary Power");
}

void ElectricalParameterPage::connectInputSignals() {
  for (auto it = m_inputWidgets.constBegin(); it != m_inputWidgets.constEnd();
       ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setParameterValue();
      double value = getParameterValue(paramName);
    });
  }
}