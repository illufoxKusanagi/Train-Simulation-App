#include "track_parameter_page.h"

TrackParameterPage::TrackParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formContainer(new QWidget(this)),
      m_formLayout(new QGridLayout(m_formContainer)) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setHorizontalSpacing(80);
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
  QList<double> values = {0, 2000, 0, 0, 0, 70};
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], ""), this);
    inputWidget->setValue(values[i]);
    m_inputWidgets[labels[i]] = inputWidget;
    m_formLayout->addWidget(inputWidget, i / 2, i % 2);
  }
}