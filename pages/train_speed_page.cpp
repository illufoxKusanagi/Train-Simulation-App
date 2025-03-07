#include "train_speed_page.h"

TrainSpeedPage::TrainSpeedPage(QWidget *parent,
                               TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  stackedWidget = new QStackedWidget(this);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainSpeedPage::setParameterValue);
  mainLayout->addWidget(stackedWidget);
  setupFirstPage();
  setupSecondPage();
  setupPagination();
  setLayout(mainLayout);
}
void TrainSpeedPage::setupFirstPage() {
  QWidget *firstPage = new QWidget(this);
  QVBoxLayout *firstPageLayout = new QVBoxLayout(firstPage);
  firstPageLayout->setSpacing(40);
  setupExactValues(firstPageLayout, "Max Speed");
  setupChart(firstPageLayout);
  stackedWidget->addWidget(firstPage);
}

void TrainSpeedPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(40);
  setupExactValues(secondPageLayout, "Static Max Speed");
  setupChart(secondPageLayout);
  stackedWidget->addWidget(secondPage);
}

void TrainSpeedPage::setupChart(QVBoxLayout *pageLayout) {
  ChartWidget *chartWidget =
      new ChartWidget("Train Speed", "speed", this, m_trainSimulation);
  pageLayout->addWidget(chartWidget);
}

void TrainSpeedPage::setupExactValues(QVBoxLayout *pageLayout,
                                      QString inputTitle) {
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter);
  InputType inputType = InputType("field", inputTitle, "km/h");
  m_inputWidget = new InputWidget(inputType, this);
  m_inputWidgets[inputTitle] = m_inputWidget;
  layout->addWidget(m_inputWidget);
  pageLayout->addLayout(layout);
}

void TrainSpeedPage::setParameterValue() {
  QList<QString> keys = m_inputWidgets.keys();

  for (const QString &key : keys) {
    if (m_inputWidgets[key]) {
      m_inputWidgets[key]->setValue(0);
    }
  }
  if (m_inputWidgets.contains("Max Speed"))
    m_inputWidgets["Max Speed"]->setValue(m_trainSimulation->findMaxSpeed());
  if (m_inputWidgets.contains("Static Max Speed"))
    m_inputWidgets["Static Max Speed"]->setValue(
        m_trainSimulation->findMaxSpeed());
}

void TrainSpeedPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction("Dynamic Speed", "false", this);
  m_nextButton = new ButtonAction("Static Speed", "false", this);
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
