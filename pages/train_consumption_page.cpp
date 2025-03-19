#include "train_consumption_page.h"

TrainConsumptionPage::TrainConsumptionPage(QWidget *parent,
                                           TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  stackedWidget = new QStackedWidget(this);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainConsumptionPage::setParameterValue);
  connect(m_trainSimulation, &TrainSimulation::staticSimulationCompleted, this,
          &TrainConsumptionPage::setStaticParameterValue);
  mainLayout->addWidget(stackedWidget);
  setupFirstPage();
  setupSecondPage();
  setupPagination();
  setLayout(mainLayout);
}
void TrainConsumptionPage::setupFirstPage() {
  QWidget *firstPage = new QWidget(this);
  QVBoxLayout *firstPageLayout = new QVBoxLayout(firstPage);
  QStringList inputLabels = {"Max Energy Consumption", "Max Energy Powering",
                             "Max Energy Regen", "Max Energy APS"};
  firstPageLayout->setSpacing(16);
  setupExactValues(firstPageLayout, inputLabels);
  setupChart(firstPageLayout, "Dynamic Energy", "Dynamic Energy");
  stackedWidget->addWidget(firstPage);
}

void TrainConsumptionPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  QStringList inputLabels = {"Max Static Energy Consumption",
                             "Max Static Energy Powering",
                             "Max Static Energy APS"};
  secondPageLayout->setSpacing(16);
  setupExactValues(secondPageLayout, inputLabels);
  setupChart(secondPageLayout, "Static Energy", "Static Energy");
  stackedWidget->addWidget(secondPage);
}

void TrainConsumptionPage::setupChart(QVBoxLayout *pageLayout,
                                      QString chartTitle, QString chartSeries) {
  ChartWidget *chartWidget =
      new ChartWidget(chartTitle, chartSeries, this, m_trainSimulation);
  m_chartWidget[chartTitle] = chartWidget;
  pageLayout->addWidget(chartWidget);
}

void TrainConsumptionPage::setupExactValues(QVBoxLayout *pageLayout,
                                            QStringList inputLabels) {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  for (const QString &inputTitles : inputLabels) {
    InputType inputType = InputType("field", inputTitles, "km/h", 0, true);
    InputWidget *inputWidget = new InputWidget(inputType, this);
    m_inputWidgets[inputTitles] = inputWidget;
    layout->addWidget(inputWidget);
  }
  pageLayout->addLayout(layout);
}

void TrainConsumptionPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction("Dynamic Energy", "false", this);
  m_nextButton = new ButtonAction("Static Energy", "false", this);
  m_prevButton->setFixedSize(144, 48);
  m_nextButton->setFixedSize(144, 48);
  connect(m_prevButton, &QPushButton::clicked, this,
          &TrainConsumptionPage::showPreviousPage);
  connect(m_nextButton, &QPushButton::clicked, this,
          &TrainConsumptionPage::showNextPage);
  paginationLayout->addWidget(m_prevButton);
  paginationLayout->addWidget(m_nextButton);
  mainLayout->addWidget(paginationWidget);
  updatePaginationButtons();
}

void TrainConsumptionPage::showPreviousPage() {
  if (stackedWidget->currentIndex() > 0) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() - 1);
    updatePaginationButtons();
  }
}

void TrainConsumptionPage::showNextPage() {
  if (stackedWidget->currentIndex() < stackedWidget->count() - 1) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() + 1);
    updatePaginationButtons();
  }
}

void TrainConsumptionPage::updatePaginationButtons() {
  int currentIndex = stackedWidget->currentIndex();
  int lastIndex = stackedWidget->count() - 1;
  m_prevButton->setEnabled(currentIndex > 0);
  m_nextButton->setEnabled(currentIndex < lastIndex);
}

void TrainConsumptionPage::updateCharts() {
  m_chartWidget["Dynamic Energy"]->updateChart();
  m_chartWidget["Static Energy"]->updateChart();
}

void TrainConsumptionPage::setParameterValue() {
  m_inputWidgets["Max Energy"]->setValue(0);
  m_inputWidgets["Max Energy"]->setValue(m_trainSimulation->findMaxSpeed());
}

void TrainConsumptionPage::setStaticParameterValue() {
  m_inputWidgets["Static Max Energy"]->setValue(0);
  m_inputWidgets["Static Max Energy"]->setValue(
      m_trainSimulation->findMaxSpeed());
}
