#include "electrical_parameter_page.h"

ElectricalParameterPage::ElectricalParameterPage(
    AppContext &context, TrainSimulationHandler *trainSimulation,
    QWidget *parent)
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
  m_electricalDataHandler = new ElectricalDataHandler(&context, this);
  createInputs();
  connect(m_trainSimulation, &TrainSimulationHandler::simulationStarted, this,
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

// getParameterValue and getCsvParamValue might be removed if no longer used
// directly by the page For now, keeping them as they are const and don't modify
// state.
// double
// ElectricalParameterPage::getParameterValue(const QString &paramName) const {
//   if (m_inputWidgets.contains(paramName)) {
//     return m_inputWidgets[paramName]->getValue();
//   }
//   return 0.0;
// }

// // getParameterValue and getCsvParamValue might be removed if no longer used
// // directly by the page For now, keeping them as they are const and don't
// modify
// // state.
// QList<double>
// ElectricalParameterPage::getCsvParamValue(const QString &paramName,
//                                           const int requiredColumn) const {
//   if (!m_inputWidgets.isEmpty()) {
//     return m_inputWidgets[paramName]->getCsvValue(requiredColumn);
//   }
//   return QList<double>();
// }

void ElectricalParameterPage::setParameterValue() {
  m_electricalDataHandler->storeFormInputs(m_inputWidgets);
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