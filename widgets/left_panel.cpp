#include "left_panel.h"

LeftPanel::LeftPanel(QWidget *parent, TrainSimulation *trainSimulation)
    : QWidget(parent), m_buttonLayout(nullptr), m_inputPanel(nullptr),
      m_trainSimulation(trainSimulation) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  m_buttonToggle = new ButtonToggle(m_isCollapsed, this);
  QWidget *buttonContainer = new QWidget(this);
  m_buttonLayout = new QVBoxLayout(buttonContainer);
  m_buttonLayout->setContentsMargins(8, 8, 8, 8);
  m_buttonLayout->setSpacing(8);
  m_buttonLayout->setAlignment(Qt::AlignTop);
  m_buttonLayout->addWidget(m_buttonToggle);

  connect(m_buttonToggle, &QPushButton::clicked, this, [this]() {
    m_isCollapsed = !m_isCollapsed;
    m_inputPanel->toggleCollapse(m_isCollapsed);
    m_outputPanel->toggleCollapse(m_isCollapsed);
    m_buttonToggle->toggleCollapse();
    m_runButtonLayout->setContentsMargins(0, 0, 0, 0);
    setFixedWidth(m_isCollapsed ? 160 : 320);
  });
  setupInputPageButtons();
  createRunButton();
  setupOutputPageButtons();
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setWidget(buttonContainer);
  scrollArea->setWidgetResizable(true);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setFrameShape(QFrame::NoFrame);
  mainLayout->addWidget(scrollArea);
  setFixedWidth(320);
  setLayout(mainLayout);
  setStyleSheet("QWidget {"
                "    background-color: " +
                Colors::Secondary500.name() +
                ";"
                "}"
                "QScrollArea {"
                "    background-color: " +
                Colors::Secondary500.name() +
                ";"
                "}");
}

void LeftPanel::emitNavigateSignal(int pageIndex) {
  emit navigateToPage(pageIndex);
}

void LeftPanel::createRunButton() {
  QWidget *runButtonWidget = new QWidget();
  m_runButtonLayout = new QVBoxLayout(runButtonWidget);
  m_runButtonLayout->setSpacing(8);
  ButtonAction *runButton = new ButtonAction("Run", "yes", this);
  runButton->setEnabled(true);
  runButton->setSize(132, 40);
  ButtonAction *runStaticButton = new ButtonAction("Static Run", "yes", this);
  runStaticButton->setEnabled(true);
  runStaticButton->setSize(132, 40);
  m_runButtonLayout->setContentsMargins(16, 0, 0, 0);
  m_runButtonLayout->setAlignment(Qt::AlignCenter);
  m_runButtonLayout->addWidget(runButton);
  m_runButtonLayout->addWidget(runStaticButton);
  connect(runButton, &ButtonAction::clicked, this,
          [this, runButton, runStaticButton]() {
            QFuture<void> future = QtConcurrent::run([this]() {
              m_trainSimulation->simulateDynamicTrainMovement();
              connect(m_trainSimulation, &TrainSimulation::simulationCompleted,
                      this, [this]() { m_trainSimulation->resetSimulation(); });
            });
            updateButtonState(future, runButton, runStaticButton);
          });
  connect(runStaticButton, &ButtonAction::clicked, this,
          [this, runButton, runStaticButton]() {
            QFuture<void> future = QtConcurrent::run([this]() {
              m_trainSimulation->simulateStaticTrainMovement();
              connect(m_trainSimulation, &TrainSimulation::simulationCompleted,
                      this, [this]() { m_trainSimulation->resetSimulation(); });
            });
            updateButtonState(future, runButton, runStaticButton);
          });
  m_buttonLayout->addWidget(runButtonWidget);
}

void LeftPanel::setupInputPageButtons() {
  m_inputPanel = new LeftPanelInputs(LeftPanelInputs::INPUT, this);
  connect(m_inputPanel, &LeftPanelInputs::buttonClicked, this,
          [this](int index) {
            m_currentIndex = index;
            emitNavigateSignal(index);
            m_inputPanel->updateCategoryButton(m_currentIndex);
            m_outputPanel->updateCategoryButton(m_currentIndex);
            m_outputPanel->setCurrentIndex(m_currentIndex - m_currentIndex - 1);
          });
  m_buttonLayout->addWidget(m_inputPanel);
}

void LeftPanel::setupOutputPageButtons() {
  m_outputPanel = new LeftPanelInputs(LeftPanelInputs::OUTPUT, this);
  connect(m_outputPanel, &LeftPanelInputs::buttonClicked, this,
          [this](int index) {
            m_currentIndex = index + 5;
            emitNavigateSignal(m_currentIndex);
            m_outputPanel->updateCategoryButton(m_currentIndex);
            m_inputPanel->updateCategoryButton(m_currentIndex);
            m_inputPanel->setCurrentIndex(m_currentIndex);
          });
  m_buttonLayout->addWidget(m_outputPanel);
}

void LeftPanel::updateButtonState(QFuture<void> future, ButtonAction *runButton,
                                  ButtonAction *runStaticButton) {
  runButton->setEnabled(false);
  runStaticButton->setEnabled(false);

  QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
  connect(watcher, &QFutureWatcher<void>::finished, this,
          [this, watcher, runStaticButton, runButton]() {
            runStaticButton->setEnabled(true);
            runButton->setEnabled(true);
            watcher->deleteLater();
          });
  watcher->setFuture(future);
}
