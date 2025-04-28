#include "train_speed_page.h"

TrainSpeedPage::TrainSpeedPage(QWidget *parent,
                               TrainSimulation *trainSimulation,
                               AppContext *context)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation), context(context) {
  mainLayout->setAlignment(Qt::AlignCenter);
  stackedWidget = new QStackedWidget(this);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainSpeedPage::setParameterValue);
  connect(m_trainSimulation, &TrainSimulation::staticSimulationCompleted, this,
          &TrainSpeedPage::setStaticParameterValue);
  mainLayout->addWidget(stackedWidget);
  setupFirstPage();
  setupSecondPage();
  setupPagination();
  setLayout(mainLayout);
}
void TrainSpeedPage::setupFirstPage() {
  QWidget *firstPage = new QWidget(this);
  QVBoxLayout *firstPageLayout = new QVBoxLayout(firstPage);
  firstPageLayout->setSpacing(16);
  setupExactValues(firstPageLayout, "Max Speed");
  setupChart(firstPageLayout, "Dynamic Speed", "Dynamic Speed");
  stackedWidget->addWidget(firstPage);
}

void TrainSpeedPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(16);
  setupExactValues(secondPageLayout, "Static Max Speed");
  setupChart(secondPageLayout, "Static Speed", "Static Speed");
  stackedWidget->addWidget(secondPage);
}

void TrainSpeedPage::setupChart(QVBoxLayout *pageLayout, QString chartTitle,
                                QString chartSeries) {
  ChartWidget *chartWidget = new ChartWidget(this, chartTitle, chartSeries,
                                             m_trainSimulation, context);
  m_chartWidget[chartTitle] = chartWidget;
  pageLayout->addWidget(chartWidget);
}

void TrainSpeedPage::setupExactValues(QVBoxLayout *pageLayout,
                                      QString inputTitle) {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", inputTitle, "km/h", true);
  m_inputWidget = new InputWidget(this, inputType);
  m_inputWidgets[inputTitle] = m_inputWidget;
  layout->addWidget(m_inputWidget);
  pageLayout->addLayout(layout);
}

void TrainSpeedPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction(this, "Dynamic Speed");
  m_nextButton = new ButtonAction(this, "Static Speed");
  m_prevButton->setFixedSize(144, 48);
  m_nextButton->setFixedSize(144, 48);
  connect(m_prevButton, &QPushButton::clicked, this,
          &TrainSpeedPage::showPreviousPage);
  connect(m_nextButton, &QPushButton::clicked, this,
          &TrainSpeedPage::showNextPage);
  paginationLayout->addWidget(m_prevButton);
  paginationLayout->addWidget(m_nextButton);
  mainLayout->addWidget(paginationWidget);
  updatePaginationButtons();
}

void TrainSpeedPage::showPreviousPage() {
  if (stackedWidget->currentIndex() > 0) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() - 1);
    updatePaginationButtons();
  }
}

void TrainSpeedPage::showNextPage() {
  if (stackedWidget->currentIndex() < stackedWidget->count() - 1) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() + 1);
    updatePaginationButtons();
  }
}

void TrainSpeedPage::updatePaginationButtons() {
  int currentIndex = stackedWidget->currentIndex();
  int lastIndex = stackedWidget->count() - 1;
  m_prevButton->setEnabled(currentIndex > 0);
  m_nextButton->setEnabled(currentIndex < lastIndex);
}

void TrainSpeedPage::setParameterValue() {
  m_inputWidgets["Max Speed"]->setValue(0);
  m_inputWidgets["Max Speed"]->setValue(
      static_cast<int>(m_trainSimulation->getMaxSpeed()));
}

void TrainSpeedPage::setStaticParameterValue() {
  m_inputWidgets["Static Max Speed"]->setValue(0);
  m_inputWidgets["Static Max Speed"]->setValue(
      static_cast<int>(m_trainSimulation->getMaxSpeed()));
}