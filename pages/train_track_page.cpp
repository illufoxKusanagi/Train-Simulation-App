#include "train_track_page.h"

TrainTrackPage::TrainTrackPage(QWidget *parent,
                               TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  stackedWidget = new QStackedWidget(this);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainTrackPage::setParameterValue);
  connect(m_trainSimulation, &TrainSimulation::staticSimulationCompleted, this,
          &TrainTrackPage::setStaticParameterValue);
  mainLayout->addWidget(stackedWidget);
  setupFirstPage();
  setupSecondPage();
  setupPagination();
  setLayout(mainLayout);
}
void TrainTrackPage::setupFirstPage() {
  QWidget *firstPage = new QWidget(this);
  QVBoxLayout *firstPageLayout = new QVBoxLayout(firstPage);
  firstPageLayout->setSpacing(16);
  setupExactValues(firstPageLayout, "Distance Travelled");
  setupChart(firstPageLayout);
  m_inputWidgets["Distance Travelled"] = m_inputWidget;
  stackedWidget->addWidget(firstPage);
}

void TrainTrackPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(16);
  setupExactValues(secondPageLayout, "Static Distance Travelled");
  setupChart(secondPageLayout);
  m_inputWidgets["Static Distance Travelled"] = m_inputWidget;
  stackedWidget->addWidget(secondPage);
}

void TrainTrackPage::setupChart(QVBoxLayout *pageLayout) {
  ChartWidget *chartWidget =
      new ChartWidget(this, "Distance", "Distance", m_trainSimulation);
  pageLayout->addWidget(chartWidget);
}

void TrainTrackPage::setupExactValues(QVBoxLayout *pageLayout,
                                      QString inputTitle) {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", inputTitle, "m", true);
  m_inputWidget = new InputWidget(this, inputType);
  m_inputWidgets[inputTitle] = m_inputWidget;
  layout->addWidget(m_inputWidget);
  pageLayout->addLayout(layout);
}

void TrainTrackPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction(this, "Dynamic Track");
  m_nextButton = new ButtonAction(this, "Static Track");
  m_prevButton->setFixedSize(144, 48);
  m_nextButton->setFixedSize(144, 48);
  connect(m_prevButton, &QPushButton::clicked, this,
          &TrainTrackPage::showPreviousPage);
  connect(m_nextButton, &QPushButton::clicked, this,
          &TrainTrackPage::showNextPage);
  paginationLayout->addWidget(m_prevButton);
  paginationLayout->addWidget(m_nextButton);
  mainLayout->addWidget(paginationWidget);
  updatePaginationButtons();
}

void TrainTrackPage::showPreviousPage() {
  if (stackedWidget->currentIndex() > 0) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() - 1);
    updatePaginationButtons();
  }
}

void TrainTrackPage::showNextPage() {
  if (stackedWidget->currentIndex() < stackedWidget->count() - 1) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() + 1);
    updatePaginationButtons();
  }
}

void TrainTrackPage::updatePaginationButtons() {
  int currentIndex = stackedWidget->currentIndex();
  int lastIndex = stackedWidget->count() - 1;
  m_prevButton->setEnabled(currentIndex > 0);
  m_nextButton->setEnabled(currentIndex < lastIndex);
}

void TrainTrackPage::setParameterValue() {
  m_inputWidgets["Distance Travelled"]->setValue(0);
  m_inputWidgets["Distance Travelled"]->setValue(
      m_trainSimulation->findDistanceTravelled());
}

void TrainTrackPage::setStaticParameterValue() {
  m_inputWidgets["Static Distance Travelled"]->setValue(0);
  m_inputWidgets["Static Distance Travelled"]->setValue(
      m_trainSimulation->findDistanceTravelled());
}
