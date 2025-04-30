#include "electrical_parameter_page.h"

ElectricalParameterPage::ElectricalParameterPage(
    AppContext &context, TrainSimulation *trainSimulation, QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formContainer(new QWidget(this)),
      m_formLayout(new QGridLayout(m_formContainer)),
      efficiencyData(context.efficiencyData.data()),
      powerData(context.powerData.data()),
      energyData(context.energyData.data()),
      m_trainSimulation(trainSimulation) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setHorizontalSpacing(128);
  m_formLayout->setVerticalSpacing(32);
  createInputs();
  connect(m_trainSimulation, &TrainSimulation::simulationStarted, this,
          [this]() { setParameterValue(); });
  mainLayout->addWidget(m_formContainer);
  setLayout(mainLayout);
}

void ElectricalParameterPage::createInputs() {
  QStringList labels = {
      "Line Voltage",       "Power Factor of Traction Motor",
      "Motor Voltage",      "Efficiency of Gearbox",
      "Efficiency of VVVF", "The Efficiency of Traction Motor",
      "Auxiliary Power"};
  QStringList units = {"V", "%", "V", "%", "%", "%", "kW"};
  QList<double> values = {1500, 0, 1200, 0.98, 0.96, 30, 0.89};
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
  efficiencyData->stat_eff_motor =
      getParameterValue("Efficiency of Traction Motor");
  efficiencyData->stat_eff_vvvf = getParameterValue("Efficiency of VVVF");
  powerData->p_aps = getParameterValue("Auxiliary Power");
  energyData->stat_vol_line = getParameterValue("Line Voltage");
  setGearboxEfficiencyValue();
  setVvvfEfficiencyValue();
  setTractionMotorValue();
  setLineVoltageValue();
  setMotorVoltageValue();
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

void ElectricalParameterPage::setGearboxEfficiencyValue() {
  QString paramName = "Efficiency of Gearbox";
  efficiencyData->stat_eff_gear = getParameterValue(paramName);
  QList<double> efficencyGearbox = getCsvParamValue(paramName, 1);
  efficiencyData->eff_gear =
      std::vector<double>(efficencyGearbox.begin(), efficencyGearbox.end());
  QList<double> efficencyGearboxSpeed = getCsvParamValue(paramName, 0);
  efficiencyData->v_eff_gear = std::vector<double>(
      efficencyGearboxSpeed.begin(), efficencyGearboxSpeed.end());
}

void ElectricalParameterPage::setVvvfEfficiencyValue() {
  QString paramName = "Efficiency of VVVF";
  efficiencyData->stat_eff_vvvf = getParameterValue(paramName);
  QList<double> efficencyVvvf = getCsvParamValue(paramName, 1);
  efficiencyData->eff_vvvf =
      std::vector<double>(efficencyVvvf.begin(), efficencyVvvf.end());
  QList<double> efficencyVvvfSpeed = getCsvParamValue(paramName, 0);
  efficiencyData->v_eff_vvvf =
      std::vector<double>(efficencyVvvfSpeed.begin(), efficencyVvvfSpeed.end());
}

void ElectricalParameterPage::setTractionMotorValue() {
  QString paramName = "The Efficiency of Traction Motor";
  efficiencyData->stat_eff_motor = getParameterValue(paramName);
  QList<double> efficencyTractionMotor = getCsvParamValue(paramName, 1);
  efficiencyData->eff_motor = std::vector<double>(
      efficencyTractionMotor.begin(), efficencyTractionMotor.end());
  QList<double> efficencyTractionMotorSpeed = getCsvParamValue(paramName, 0);
  efficiencyData->v_eff_motor = std::vector<double>(
      efficencyTractionMotorSpeed.begin(), efficencyTractionMotorSpeed.end());
}

void ElectricalParameterPage::setLineVoltageValue() {
  QString paramName = "Line Voltage";
  energyData->stat_vol_line = getParameterValue(paramName);
  QList<double> lineVoltage = getCsvParamValue(paramName, 1);
  energyData->vol_line =
      std::vector<double>(lineVoltage.begin(), lineVoltage.end());
  QList<double> lineVoltageSpeed = getCsvParamValue(paramName, 0);
  energyData->v_vol_line =
      std::vector<double>(lineVoltageSpeed.begin(), lineVoltageSpeed.end());
}

void ElectricalParameterPage::setMotorVoltageValue() {
  QString paramName = "Motor Voltage";
  energyData->stat_vol_motor = getParameterValue(paramName);
  QList<double> motorVoltage = getCsvParamValue(paramName, 1);
  energyData->vol_motor =
      std::vector<double>(motorVoltage.begin(), motorVoltage.end());
  QList<double> motorVoltageSpeed = getCsvParamValue(paramName, 0);
  energyData->v_vol_motor =
      std::vector<double>(motorVoltageSpeed.begin(), motorVoltageSpeed.end());
}