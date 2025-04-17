#include "traction_effort_page.h"

TractionEffortPage::TractionEffortPage(QWidget *parent,
                                       TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TractionEffortPage::setParameterValue);
  connect(m_trainSimulation, &TrainSimulation::staticSimulationCompleted, this,
          &TractionEffortPage::setStaticParameterValue);
  stackedWidget = new QStackedWidget(this);
  mainLayout->addWidget(stackedWidget);
  setupFirstPage();
  setupSecondPage();
  setupPagination();
  setLayout(mainLayout);
}

void TractionEffortPage::setupFirstPage() {
  QWidget *firstPage = new QWidget(this);
  QVBoxLayout *firstPageLayout = new QVBoxLayout(firstPage);
  firstPageLayout->setSpacing(16);
  setupExactValue(firstPageLayout, "Max Dynamic Traction Effort");
  setupChart(firstPageLayout, "Dynamic Traction Effort", "Dynamic F Motor");
  stackedWidget->addWidget(firstPage);
}

void TractionEffortPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(16);
  setupExactValue(secondPageLayout, "Max Static Traction Effort");
  setupChart(secondPageLayout, "Static Traction Effort", "Static F Motor");
  stackedWidget->addWidget(secondPage);
}

void TractionEffortPage::setupChart(QVBoxLayout *pageLayout, QString chartTitle,
                                    QString chartSeries) {
  ChartWidget *chartWidget =
      new ChartWidget(this, chartTitle, chartSeries, m_trainSimulation);
  m_chartWidget[chartTitle] = chartWidget;
  pageLayout->addWidget(chartWidget);
}

void TractionEffortPage::setupExactValue(QVBoxLayout *pageLayout,
                                         QString inputTitle) {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", inputTitle, "kN", true);
  m_inputWidget = new InputWidget(this, inputType);
  m_inputWidgets[inputTitle] = m_inputWidget;
  layout->addWidget(m_inputWidget);
  if (inputTitle.contains("Static")) {
    InputType adhesionInputType = InputType("field", "Adhesion", "kN", true);
    InputWidget *adhesionInput = new InputWidget(this, adhesionInputType);
    m_inputWidgets["Adhesion"] = adhesionInput;
    layout->addWidget(adhesionInput);
  }
  pageLayout->addLayout(layout);
}

void TractionEffortPage::setParameterValue() {
  m_inputWidgets["Max Dynamic Traction Effort"]->setValue(0);
  m_inputWidgets["Max Dynamic Traction Effort"]->setValue(
      m_trainSimulation->getMaxTractionEffort());
}

void TractionEffortPage::setStaticParameterValue() {
  m_inputWidgets["Max Static Traction Effort"]->setValue(0);
  m_inputWidgets["Max Static Traction Effort"]->setValue(
      m_trainSimulation->getMaxTractionEffort());
  m_inputWidgets["Adhesion"]->setValue(0);
  m_inputWidgets["Adhesion"]->setValue(m_trainSimulation->getAdhesion());
}

void TractionEffortPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction(this, "Dynamic Traction");
  m_nextButton = new ButtonAction(this, "Static Traction");
  m_prevButton->setFixedSize(144, 48);
  m_nextButton->setFixedSize(144, 48);
  connect(m_prevButton, &QPushButton::clicked, this,
          &TractionEffortPage::showPreviousPage);
  connect(m_nextButton, &QPushButton::clicked, this,
          &TractionEffortPage::showNextPage);
  paginationLayout->addWidget(m_prevButton);
  paginationLayout->addWidget(m_nextButton);
  mainLayout->addWidget(paginationWidget);
  updatePaginationButtons();
}

void TractionEffortPage::showPreviousPage() {
  if (stackedWidget->currentIndex() > 0) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() - 1);
    updatePaginationButtons();
  }
}

void TractionEffortPage::showNextPage() {
  if (stackedWidget->currentIndex() < stackedWidget->count() - 1) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() + 1);
    updatePaginationButtons();
  }
}

void TractionEffortPage::updatePaginationButtons() {
  int currentIndex = stackedWidget->currentIndex();
  int lastIndex = stackedWidget->count() - 1;
  m_prevButton->setEnabled(currentIndex > 0);
  m_nextButton->setEnabled(currentIndex < lastIndex);
}