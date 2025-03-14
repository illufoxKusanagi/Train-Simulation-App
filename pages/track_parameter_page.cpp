#include "track_parameter_page.h"

TrackParameterPage::TrackParameterPage(QWidget *parent, MovingData *movingData,
                                       ResistanceData *resistanceData)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formContainer(new QWidget(this)),
      m_formLayout(new QGridLayout(m_formContainer)),
      resistanceData(resistanceData), movingData(movingData) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setHorizontalSpacing(128);
  m_formLayout->setVerticalSpacing(32);
  createInputs();
  mainLayout->addWidget(m_formContainer);
  setLayout(mainLayout);
}

void TrackParameterPage::createInputs() {
  QStringList labels = {"Number of Station", "Radius per Section",
                        "Station Distance",  "Slope per Section",
                        "Section Distance",  "Speed Limit"};
  QStringList units = {"m/s2", "m", "m", "m", "‰", "km/h"};
  QList<double> values = {0.0, 2000, 0.0, 0.0, 0.0, 70};
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], ""), this);
    inputWidget->setValue(values[i]);
    inputWidget->setFixedHeight(80);
    m_formLayout->addWidget(inputWidget, i / 2, i % 2);
    m_inputWidgets[labels[i]] = inputWidget;
  }
  setParameterValue();
  connectInputSignals();
}

double TrackParameterPage::getParameterValue(const QString &paramName) const {
  if (m_inputWidgets.contains(paramName)) {
    return m_inputWidgets[paramName]->getValue();
  }
  return 0.0;
}

void TrackParameterPage::setParameterValue() {
  movingData->v_limit = getParameterValue("Speed Limit");
  resistanceData->radius = getParameterValue("Radius per Section");
  resistanceData->slope = getParameterValue("Slope per Section");
}

void TrackParameterPage::connectInputSignals() {
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