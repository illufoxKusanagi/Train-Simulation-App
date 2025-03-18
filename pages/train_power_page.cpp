#include "train_power_page.h"

TrainPowerPage::TrainPowerPage(QWidget *parent,
                               TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation) {
  mainLayout->setAlignment(Qt::AlignCenter);
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &TrainPowerPage::setParameterValue);
  connect(m_trainSimulation, &TrainSimulation::staticSimulationCompleted, this,
          &TrainPowerPage::setStaticParameterValue);
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
  firstPageLayout->setSpacing(16);
  QStringList labels = {"Dynamic Catenary Power", "Dynamic VVVF Power"};
  setupInputs(firstPageLayout, labels);
  setupChart(firstPageLayout, "Dynamic Power", "Dynamic Power");
  stackedWidget->addWidget(firstPage);
}

void TrainPowerPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  secondPageLayout->setSpacing(16);
  QStringList labels = {"Dynamic Catenary Current", "Dynamic VVVF Current"};
  setupInputs(secondPageLayout, labels);
  setupChart(secondPageLayout, "Dynamic Current", "Dynamic Current");
  stackedWidget->addWidget(secondPage);
}

void TrainPowerPage::setupThirdPage() {
  QWidget *thirdPage = new QWidget(this);
  QVBoxLayout *thirdPageLayout = new QVBoxLayout(thirdPage);
  thirdPageLayout->setSpacing(16);
  QStringList labels = {
      "Static Catenary Power",
      "Static VVVF Power",
  };
  setupInputs(thirdPageLayout, labels);
  setupChart(thirdPageLayout, "Static Power", "Static Power");
  stackedWidget->addWidget(thirdPage);
}

void TrainPowerPage::setupFourthPage() {
  QWidget *fourthPage = new QWidget(this);
  QVBoxLayout *fourthPageLayout = new QVBoxLayout(fourthPage);
  fourthPageLayout->setSpacing(16);
  QStringList labels = {
      "Static Catenary Current",
      "Static VVVF Current",
  };
  setupInputs(fourthPageLayout, labels);
  setupChart(fourthPageLayout, "Static Current", "Static Current");
  stackedWidget->addWidget(fourthPage);
}

void TrainPowerPage::setupChart(QVBoxLayout *pageLayout, QString chartTitle,
                                QString chartSeries) {
  ChartWidget *chartWidget =
      new ChartWidget(chartTitle, chartSeries, this, m_trainSimulation);
  m_chartWidgets[chartTitle] = chartWidget;
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
  QList<QString> keys = m_inputWidgets.keys();

  for (const QString &key : keys) {
    if (m_inputWidgets[key] && key.contains("Dynamic")) {
      m_inputWidgets[key]->setValue(0);
    }
  }
  if (m_inputWidgets.contains("Dynamic Catenary Power"))
    m_inputWidgets["Dynamic Catenary Power"]->setValue(
        m_trainSimulation->findMaxCatenaryPower());
  if (m_inputWidgets.contains("Dynamic VVVF Power"))
    m_inputWidgets["Dynamic VVVF Power"]->setValue(
        m_trainSimulation->findMaxVvvfPower());
  if (m_inputWidgets.contains("Dynamic Catenary Current"))
    m_inputWidgets["Dynamic Catenary Current"]->setValue(
        m_trainSimulation->findMaxCatenaryCurrent());
  if (m_inputWidgets.contains("Dynamic VVVF Current"))
    m_inputWidgets["Dynamic VVVF Current"]->setValue(
        m_trainSimulation->findMaxVvvfCurrent());
}

void TrainPowerPage::setStaticParameterValue() {
  QList<QString> keys = m_inputWidgets.keys();

  for (const QString &key : keys) {
    if (m_inputWidgets[key] && key.contains("Static")) {
      m_inputWidgets[key]->setValue(0);
    }
  }
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

  m_firstPageButton = new ButtonAction("Dynamic Power", "normal", this);
  m_secondPageButton = new ButtonAction("Dynamic Current", "normal", this);
  m_thirdPageButton = new ButtonAction("Static Power", "normal", this);
  m_fourthPageButton = new ButtonAction("Static Current", "normal", this);

  m_firstPageButton->setFixedSize(144, 48);
  m_secondPageButton->setFixedSize(144, 48);
  m_thirdPageButton->setFixedSize(144, 48);
  m_fourthPageButton->setFixedSize(144, 48);

  connect(m_firstPageButton, &QPushButton::clicked, this,
          [this]() { pageChanged(0); });
  connect(m_secondPageButton, &QPushButton::clicked, this,
          [this]() { pageChanged(1); });
  connect(m_thirdPageButton, &QPushButton::clicked, this,
          [this]() { pageChanged(2); });
  connect(m_fourthPageButton, &QPushButton::clicked, this,
          [this]() { pageChanged(3); });
  paginationLayout->addWidget(m_firstPageButton);
  paginationLayout->addWidget(m_thirdPageButton);
  paginationLayout->addWidget(m_fourthPageButton);
  paginationLayout->addWidget(m_secondPageButton);

  mainLayout->addWidget(paginationWidget);
  updatePageButtons();
}

void TrainPowerPage::pageChanged(int pageIndex) {
  int maxIndex = stackedWidget->count() - 1;
  pageIndex = qBound(0, pageIndex, maxIndex);
  stackedWidget->setCurrentIndex(pageIndex);
  updatePageButtons();
}

void TrainPowerPage::updateCharts() {
  m_chartWidgets["Dynamic Power"]->updateChart();
  m_chartWidgets["Dynamic Current"]->updateChart();
  m_chartWidgets["Static Power"]->updateChart();
  m_chartWidgets["Static Current"]->updateChart();
}

void TrainPowerPage::updatePageButtons() {
  int currentIndex = stackedWidget->currentIndex();
  m_firstPageButton->setEnabled(currentIndex != 0);
  m_secondPageButton->setEnabled(currentIndex != 1);
  m_thirdPageButton->setEnabled(currentIndex != 2);
  m_fourthPageButton->setEnabled(currentIndex != 3);
}