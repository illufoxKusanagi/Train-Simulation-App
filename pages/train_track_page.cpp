#include "train_track_page.h"

TrainTrackPage::TrainTrackPage(QWidget *parent,
                               TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  stackedWidget = new QStackedWidget(this);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainTrackPage::setParameterValue);
  mainLayout->addWidget(stackedWidget);
  setupFirstPage();
  setupSecondPage();
  setupPagination();
  setLayout(mainLayout);
}
void TrainTrackPage::setupFirstPage() {
  QWidget *firstPage = new QWidget(this);
  QVBoxLayout *firstPageLayout = new QVBoxLayout(firstPage);
  firstPageLayout->setSpacing(40);
  setupExactValues(firstPageLayout, "Distance Travelled");
  setupChart(firstPageLayout);
  stackedWidget->addWidget(firstPage);
}

void TrainTrackPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(40);
  // setupExactValues(secondPageLayout, "Max Distance Travelled");
  setupChart(secondPageLayout);
  stackedWidget->addWidget(secondPage);
}

void TrainTrackPage::setupChart(QVBoxLayout *pageLayout) {
  ChartWidget *chartWidget =
      new ChartWidget("Distance", "speed", this, m_trainSimulation);
  pageLayout->addWidget(chartWidget);
}

void TrainTrackPage::setupExactValues(QVBoxLayout *pageLayout,
                                      QString inputTitle) {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", inputTitle, "m", 0, true);
  m_inputWidget = new InputWidget(inputType, this);
  layout->addWidget(m_inputWidget);
  pageLayout->addLayout(layout);
}

void TrainTrackPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction("Dynamic Track", "false", this);
  m_nextButton = new ButtonAction("Static Track", "false", this);
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
  m_inputWidget->setValue(0);
  m_inputWidget->setValue(m_trainSimulation->findDistanceTravelled());
}
