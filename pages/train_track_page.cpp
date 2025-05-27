#include "train_track_page.h"

TrainTrackPage::TrainTrackPage(QWidget *parent,
                               TrainSimulationHandler *trainSimulation,
                               AppContext *context)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation), context(context) {
  mainLayout->setAlignment(Qt::AlignCenter);
  stackedWidget = new QStackedWidget(this);
  connect(m_trainSimulation, &TrainSimulationHandler::simulationCompleted, this,
          &TrainTrackPage::setParameterValue);
  connect(m_trainSimulation, &TrainSimulationHandler::staticSimulationCompleted,
          this, &TrainTrackPage::setStaticParameterValue);
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
  setupExactValues(firstPageLayout, {"Distance Travelled"});
  setupChart(firstPageLayout, "Distance");
  stackedWidget->addWidget(firstPage);
}

void TrainTrackPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(16);
  QStringList inputTitles = {"Distance on Powering", "Distance on Braking",
                             "Distance on Emergency Braking"};
  setupExactValues(secondPageLayout, inputTitles);
  setupChart(secondPageLayout, "Static Track");
  stackedWidget->addWidget(secondPage);
}

void TrainTrackPage::setupChart(QVBoxLayout *pageLayout, QString chartTitle) {
  ChartWidget *chartWidget =
      new ChartWidget(this, chartTitle, "Distance", m_trainSimulation, context);
  pageLayout->addWidget(chartWidget);
}

void TrainTrackPage::setupExactValues(QVBoxLayout *pageLayout,
                                      QStringList inputTitle) {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  for (int i = 0; i < inputTitle.size(); i++) {
    InputType inputType = InputType("field", inputTitle[i], "m", true);
    InputWidget *inputWidget = new InputWidget(this, inputType);
    inputWidget->setFixedHeight(80);
    layout->addWidget(inputWidget);
    m_inputWidgets[inputTitle[i]] = inputWidget;
  }
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
      m_trainSimulation->getDistanceTravelled());
}

void TrainTrackPage::setStaticParameterValue() {
  m_inputWidgets["Distance on Powering"]->setValue(0);
  m_inputWidgets["Distance on Powering"]->setValue(
      m_trainSimulation->getDistanceTravelled());
  m_inputWidgets["Distance on Braking"]->setValue(0);
  m_inputWidgets["Distance on Braking"]->setValue(
      m_trainSimulation->m_simulationTrackHandler->calculateBrakingTrack(
          context->movingData->v_limit));
  m_inputWidgets["Distance on Emergency Braking"]->setValue(0);
  m_inputWidgets["Distance on Emergency Braking"]->setValue(
      m_trainSimulation->m_simulationTrackHandler
          ->calculateBrakingEmergencyTrack());
}
