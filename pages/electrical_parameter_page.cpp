#include "electrical_parameter_page.h"

ElectricalParameterPage::ElectricalParameterPage(AppContext &context,
                                                 QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formContainer(new QWidget(this)),
      m_formLayout(new QGridLayout(m_formContainer)),
      efficiencyData(context.efficiencyData.data()),
      powerData(context.powerData.data()),
      energyData(context.energyData.data()) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setHorizontalSpacing(128);
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
  QStringList units = {"V", "%", "V", "%", "%", "%", "kW"};
  QList<double> values = {1500, 0, 0, 0.98, 0.96, 30, 0.89};
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(this, InputType(i == 6 ? "field" : "field upload",
                                        labels[i], units[i]));
    inputWidget->setValue(values[i]);
    inputWidget->setMinimumHeight(80);
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
  efficiencyData->stat_eff_gear = getParameterValue("Efficiency of Gearbox");
  efficiencyData->stat_eff_motor =
      getParameterValue("Efficiency of Traction Motor");
  efficiencyData->stat_eff_vvvf = getParameterValue("Efficiency of VVVF");
  powerData->p_aps = getParameterValue("Auxiliary Power");
  energyData->stat_vol_line = getParameterValue("Line Voltage");
}

void ElectricalParameterPage::connectInputSignals() {
  for (auto it = m_inputWidgets.constBegin(); it != m_inputWidgets.constEnd();
       ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this,
            [this, paramName]() { setParameterValue(); });
  }
}

QList<double>
ElectricalParameterPage::getCsvParamValue(const QString &paramName,
                                          const int requiredColumn) const {
  if (!m_inputWidgets.isEmpty()) {
    return m_inputWidgets[paramName]->getCsvValue(requiredColumn);
  }
  return QList<double>();
}
