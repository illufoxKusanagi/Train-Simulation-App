#include "train_power_page.h"

TrainPowerPage::TrainPowerPage(QWidget *parent,
                               TrainSimulation *trainSimulation,
                               AppContext *context)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_trainSimulation(trainSimulation), context(context) {
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
  QStringList units = {"kW", "kW"};
  firstPageLayout->setSpacing(16);
  QStringList labels = {"Dynamic Catenary Power", "Dynamic VVVF Power"};
  setupInputs(firstPageLayout, labels, units);
  setupChart(firstPageLayout, "Dynamic Power", "Dynamic Power");
  stackedWidget->addWidget(firstPage);
}

void TrainPowerPage::setupSecondPage() {
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  QStringList units = {"Ampere", "Ampere"};
  secondPageLayout->setSpacing(16);
  QStringList labels = {"Dynamic Catenary Current", "Dynamic VVVF Current"};
  setupInputs(secondPageLayout, labels, units);
  setupChart(secondPageLayout, "Dynamic Current", "Dynamic Current");
  stackedWidget->addWidget(secondPage);
}

void TrainPowerPage::setupThirdPage() {
  QWidget *thirdPage = new QWidget(this);
  QVBoxLayout *thirdPageLayout = new QVBoxLayout(thirdPage);
  QStringList units = {"kW", "kW", "s"};
  thirdPageLayout->setSpacing(16);
  QStringList labels = {"Static Catenary Power", "Static VVVF Power",
                        "Time at Maximum Power"};
  setupInputs(thirdPageLayout, labels, units);
  setupChart(thirdPageLayout, "Static Power", "Static Power");
  stackedWidget->addWidget(thirdPage);
}

void TrainPowerPage::setupFourthPage() {
  QWidget *fourthPage = new QWidget(this);
  QVBoxLayout *fourthPageLayout = new QVBoxLayout(fourthPage);
  QStringList units = {"Ampere", "Ampere", "s"};
  fourthPageLayout->setSpacing(16);
  QStringList labels = {"Static Catenary Current", "Static VVVF Current",
                        "Time at Maximum Current"};
  setupInputs(fourthPageLayout, labels, units);
  setupChart(fourthPageLayout, "Static Current", "Static Current");
  stackedWidget->addWidget(fourthPage);
}

void TrainPowerPage::setupChart(QVBoxLayout *pageLayout, QString chartTitle,
                                QString chartSeries) {
  ChartWidget *chartWidget = new ChartWidget(this, chartTitle, chartSeries,
                                             m_trainSimulation, context);
  m_chartWidgets[chartTitle] = chartWidget;
  pageLayout->addWidget(chartWidget);
}

void TrainPowerPage::setupInputs(QVBoxLayout *pageLayout,
                                 QStringList inputTitle,
                                 QStringList inputUnit) {
  QHBoxLayout *exactValueLayout = new QHBoxLayout;
  exactValueLayout->setSpacing(16);
  for (int i = 0; i < inputTitle.size(); i++) {
    InputWidget *inputWidget = new InputWidget(
        this, InputType("field", inputTitle[i], inputUnit[i], true));
    inputWidget->setFixedHeight(80);
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
        m_trainSimulation->getMaxCatenaryPower());
  if (m_inputWidgets.contains("Dynamic VVVF Power"))
    m_inputWidgets["Dynamic VVVF Power"]->setValue(
        m_trainSimulation->getMaxVvvfPower());
  if (m_inputWidgets.contains("Dynamic Catenary Current"))
    m_inputWidgets["Dynamic Catenary Current"]->setValue(
        m_trainSimulation->getMaxCatenaryCurrent());
  if (m_inputWidgets.contains("Dynamic VVVF Current"))
    m_inputWidgets["Dynamic VVVF Current"]->setValue(
        m_trainSimulation->getMaxVvvfCurrent());
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
        m_trainSimulation->getMaxCatenaryPower());
  if (m_inputWidgets.contains("Static VVVF Power"))
    m_inputWidgets["Static VVVF Power"]->setValue(
        m_trainSimulation->getMaxVvvfPower());
  if (m_inputWidgets.contains("Static Catenary Current"))
    m_inputWidgets["Static Catenary Current"]->setValue(
        m_trainSimulation->getMaxCatenaryCurrent());
  if (m_inputWidgets.contains("Static VVVF Current"))
    m_inputWidgets["Static VVVF Current"]->setValue(
        m_trainSimulation->getMaxVvvfCurrent());
  if (m_inputWidgets["Time at Maximum Power"])
    m_inputWidgets["Time at Maximum Power"]->setValue(
        m_trainSimulation->getMaxPowTime());
  if (m_inputWidgets["Time at Maximum Current"])
    m_inputWidgets["Time at Maximum Current"]->setValue(
        m_trainSimulation->getMaxCurrTime());
}

void TrainPowerPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);

  m_firstPageButton = new ButtonAction(this, "Dynamic Power");
  m_secondPageButton = new ButtonAction(this, "Dynamic Current");
  m_thirdPageButton = new ButtonAction(this, "Static Power");
  m_fourthPageButton = new ButtonAction(this, "Static Current");

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
  paginationLayout->addWidget(m_secondPageButton);
  paginationLayout->addWidget(m_thirdPageButton);
  paginationLayout->addWidget(m_fourthPageButton);

  mainLayout->addWidget(paginationWidget);
  updatePageButtons();
}

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