#include "traction_effort_page.h"

TractionEffortPage::TractionEffortPage(QWidget *parent,
                                       TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TractionEffortPage::setParameterValue);
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
  firstPageLayout->setSpacing(40);
  setupExactValue(firstPageLayout, "Max Traction Effort");
  setupChart(firstPageLayout);
  stackedWidget->addWidget(firstPage);
}

void TractionEffortPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(40);
  setupExactValue(secondPageLayout, "Max Static Traction Effort");
  setupChart(secondPageLayout);
  stackedWidget->addWidget(secondPage);
}

void TractionEffortPage::setupChart(QVBoxLayout *pageLayout) {
  ChartWidget *chartWidget =
      new ChartWidget("Traction Effort", "speed", this, m_trainSimulation);
  pageLayout->addWidget(chartWidget);
}

void TractionEffortPage::setupExactValue(QVBoxLayout *pageLayout,
                                         QString inputTitle) {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", inputTitle, "kN");
  m_inputWidget = new InputWidget(inputType, this);
  m_inputWidgets[inputTitle] = m_inputWidget;
  layout->addWidget(m_inputWidget);
  pageLayout->addLayout(layout);
}

void TractionEffortPage::setParameterValue() {
  QList<QString> keys = m_inputWidgets.keys();

  for (const QString &key : keys) {
    if (m_inputWidgets[key]) {
      m_inputWidgets[key]->setValue(0);
    }
  }
  if (m_inputWidgets.contains("Max Traction Effort"))
    m_inputWidgets["Max Traction Effort"]->setValue(
        m_trainSimulation->findMaxTractionEffort());
  if (m_inputWidgets.contains("Max Static Traction Effort"))
    m_inputWidgets["Max Static Traction Effort"]->setValue(
        m_trainSimulation->findMaxTractionEffort());
}

void TractionEffortPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction("Dynamic Traction", "false", this);
  m_nextButton = new ButtonAction("Static Traction", "false", this);
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