#include "traction_effort_page.h"

TractionEffortPage::TractionEffortPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_stackedWidget(new QStackedWidget(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setSpacing(16);
  setupChart();
  setupExactValue();
  ChartWidget *chartWidget = new ChartWidget("Traction Effort", "speed", this);
  mainLayout->addWidget(m_stackedWidget);
  mainLayout->addWidget(chartWidget);
  setLayout(mainLayout);
}

void TractionEffortPage::setupChart() {
  // m_stackedWidget->addWidget(chartWidget);
}

void TractionEffortPage::setupExactValue() {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", "Max Traction Effort", "kN");
  InputWidget *inputWidget = new InputWidget(inputType, this);
  layout->addWidget(inputWidget);
  mainLayout->addLayout(layout);
  // m_stackedWidget->addWidget(inputWidget);
}
