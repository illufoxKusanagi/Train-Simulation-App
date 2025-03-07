#include "train_power_page.h"

TrainPowerPage::TrainPowerPage(QWidget *parent,
                               TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainPowerPage::setParameterValue);
  stackedWidget = new QStackedWidget(this);
  mainLayout->addWidget(stackedWidget);
  setupFirstPage();
  setupSecondPage();
  setupThirdPage();
  setupFourthPage();
  setupPagination();
  setLayout(mainLayout);
}

void TrainPowerPage::setupFirstPage() {
  QWidget *firstPage = new QWidget(this);
  QVBoxLayout *firstPageLayout = new QVBoxLayout(firstPage);
  firstPageLayout->setSpacing(40);
  QStringList labels = {"Dynamic Catenary Power", "Dynamic Max VVVF Power"};
  setupInputs(firstPageLayout, labels);
  setupChart(firstPageLayout);
  stackedWidget->addWidget(firstPage);
}

void TrainPowerPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(40);
  QStringList labels = {"Dynamic Catenary Current", "Dynamic VVVF Current"};
  setupInputs(secondPageLayout, labels);
  setupChart(secondPageLayout);
  stackedWidget->addWidget(secondPage);
}

void TrainPowerPage::setupThirdPage() {
  QWidget *thirdPage = new QWidget(this);
  QVBoxLayout *thirdPageLayout = new QVBoxLayout(thirdPage);
  thirdPageLayout->setSpacing(40);
  QStringList labels = {
      "Static Catenary Power",
      "Static VVVF Power",
  };
  setupInputs(thirdPageLayout, labels);
  setupChart(thirdPageLayout);
  stackedWidget->addWidget(thirdPage);
}

void TrainPowerPage::setupFourthPage() {
  QWidget *fourthPage = new QWidget(this);
  QVBoxLayout *fourthPageLayout = new QVBoxLayout(fourthPage);
  fourthPageLayout->setSpacing(40);
  QStringList labels = {
      "Static Catenary Current",
      "Static VVVF Current",
  };
  setupInputs(fourthPageLayout, labels);
  setupChart(fourthPageLayout);
  stackedWidget->addWidget(fourthPage);
}

void TrainPowerPage::setupChart(QVBoxLayout *pageLayout) {
  ChartWidget *chartWidget =
      new ChartWidget("Train Power", "speed", this, m_trainSimulation);
  pageLayout->addWidget(chartWidget);
}

void TrainPowerPage::setupInputs(QVBoxLayout *pageLayout,
                                 QStringList inputTitle) {
  QHBoxLayout *exactValueLayout = new QHBoxLayout;
  QStringList units = {"kW", "kW", "Ampere", "Ampere"};
  for (int i = 0; i < inputTitle.size(); i++) {
    InputWidget *inputWidget = new InputWidget(
        InputType("field", inputTitle[i], units[i], 0, true), this);
    exactValueLayout->addWidget(inputWidget);
    m_inputWidgets[inputTitle[i]] = inputWidget;
  }
  exactValueLayout->setAlignment(Qt::AlignCenter);
  exactValueLayout->setSpacing(160);
  pageLayout->addLayout(exactValueLayout);
}

void TrainPowerPage::setParameterValue() {
  // First check if each key exists before accessing
  QList<QString> keys = m_inputWidgets.keys();

  // Reset all values first
  for (const QString &key : keys) {
    if (m_inputWidgets[key]) {
      m_inputWidgets[key]->setValue(0);
    }
  }

  // Set new values only for widgets that exist
  if (m_inputWidgets.contains("Dynamic Catenary Power"))
    m_inputWidgets["Dynamic Catenary Power"]->setValue(
        m_trainSimulation->findMaxCatenaryPower());

  if (m_inputWidgets.contains("Dynamic Max VVVF Power"))
    m_inputWidgets["Dynamic Max VVVF Power"]->setValue(
        m_trainSimulation->findMaxVvvfPower());

  if (m_inputWidgets.contains("Dynamic Catenary Current"))
    m_inputWidgets["Dynamic Catenary Current"]->setValue(
        m_trainSimulation->findMaxCatenaryCurrent());

  if (m_inputWidgets.contains("Dynamic VVVF Current"))
    m_inputWidgets["Dynamic VVVF Current"]->setValue(
        m_trainSimulation->findMaxVvvfCurrent());

  // Static values
  if (m_inputWidgets.contains("Static Catenary Power"))
    m_inputWidgets["Static Catenary Power"]->setValue(
        m_trainSimulation->findMaxCatenaryPower());

  if (m_inputWidgets.contains("Static VVVF Power"))
    m_inputWidgets["Static VVVF Power"]->setValue(
        m_trainSimulation->findMaxVvvfPower());

  if (m_inputWidgets.contains("Static Catenary Current"))
    m_inputWidgets["Static Catenary Current"]->setValue(
        m_trainSimulation->findMaxCatenaryCurrent());

  if (m_inputWidgets.contains("Static VVVF Current"))
    m_inputWidgets["Static VVVF Current"]->setValue(
        m_trainSimulation->findMaxVvvfCurrent());
}

void TrainPowerPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);

  // Create direct page buttons
  m_firstPageButton = new ButtonAction("Dynamic Power", "normal", this);
  m_secondPageButton = new ButtonAction("Dynamic Current", "normal", this);
  m_thirdPageButton = new ButtonAction("Static Power", "normal", this);
  m_fourthPageButton = new ButtonAction("Static Current", "normal", this);

  // Set fixed size for all buttons
  m_firstPageButton->setFixedSize(144, 48);
  m_secondPageButton->setFixedSize(144, 48);
  m_thirdPageButton->setFixedSize(144, 48);
  m_fourthPageButton->setFixedSize(144, 48);

  // Connect buttons to changePage with their respective page indices
  connect(m_firstPageButton, &QPushButton::clicked, this,
          [this]() { pageChanged(0); });
  connect(m_secondPageButton, &QPushButton::clicked, this,
          [this]() { pageChanged(1); });
  connect(m_thirdPageButton, &QPushButton::clicked, this,
          [this]() { pageChanged(2); });
  connect(m_fourthPageButton, &QPushButton::clicked, this,
          [this]() { pageChanged(3); });

  // Add buttons to layout
  paginationLayout->addWidget(m_firstPageButton);
  paginationLayout->addWidget(m_thirdPageButton);
  paginationLayout->addWidget(m_fourthPageButton);
  paginationLayout->addWidget(m_secondPageButton);

  // Only add these if you actually have 3rd and 4th pages

  mainLayout->addWidget(paginationWidget);

  // Initialize button states
  updatePageButtons();
}

// void TrainPowerPage::showPreviousPage() {
//   if (stackedWidget->currentIndex() > 0) {
//     stackedWidget->setCurrentIndex(stackedWidget->currentIndex() - 1);
//     updatePaginationButtons();
//   }
// }

// void TrainPowerPage::showNextPage() {
//   if (stackedWidget->currentIndex() < stackedWidget->count() - 1) {
//     stackedWidget->setCurrentIndex(stackedWidget->currentIndex() + 1);
//     updatePaginationButtons();
//   }
// }

// void TrainPowerPage::updatePaginationButtons() {
//   int currentIndex = stackedWidget->currentIndex();
//   int lastIndex = stackedWidget->count() - 1;
//   m_prevButton->setEnabled(currentIndex > 0);
//   m_nextButton->setEnabled(currentIndex < lastIndex);
// }

void TrainPowerPage::pageChanged(int pageIndex) {
  int maxIndex = stackedWidget->count() - 1;
  pageIndex = qBound(0, pageIndex, maxIndex);
  stackedWidget->setCurrentIndex(pageIndex);
  updatePageButtons();
}

void TrainPowerPage::updatePageButtons() {
  int currentIndex = stackedWidget->currentIndex();
  m_firstPageButton->setEnabled(currentIndex != 0);
  m_secondPageButton->setEnabled(currentIndex != 1);
  m_thirdPageButton->setEnabled(currentIndex != 2);
  m_fourthPageButton->setEnabled(currentIndex != 3);
}