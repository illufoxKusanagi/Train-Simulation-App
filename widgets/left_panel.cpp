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
  m_buttonLayout->setAlignment(Qt::AlignCenter);
  m_buttonLayout->addWidget(m_buttonToggle);

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
  // connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
  //         &LeftPanel::onSimulationCompleted);
  // connect(m_trainSimulation, &TrainSimulation::staticSimulationCompleted,
  // this,
  //         &LeftPanel::onSimulationCompleted);
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
                "}"
                "QFrame[frameShape=\"4\"] {"
                "    background-color: " +
                Colors::Secondary300.name() +
                ";"
                "    color: transparent;"
                "}");
}

void LeftPanel::emitNavigateSignal(int pageIndex) {
  emit navigateToPage(pageIndex);
}

void LeftPanel::createRunButton() {
  QWidget *runButtonWidget = new QWidget();
  QStackedLayout *stackedLayout = new QStackedLayout(runButtonWidget);
  stackedLayout->setSpacing(8);

  ButtonAction *runButtonV = new ButtonAction(this, "Run");
  runButtonV->setEnabled(true);
  runButtonV->setSize(132, 40);

  ButtonAction *runStaticButtonV = new ButtonAction(this, "Static Run");
  runStaticButtonV->setEnabled(true);
  runStaticButtonV->setSize(132, 40);

  ButtonAction *runButtonH = new ButtonAction(this, "Run");
  runButtonH->setEnabled(true);
  runButtonH->setSize(132, 40);

  ButtonAction *runStaticButtonH = new ButtonAction(this, "Static Run");
  runStaticButtonH->setEnabled(true);
  runStaticButtonH->setSize(132, 40);

  QWidget *verticalContainer = new QWidget(runButtonWidget);
  m_runVButtonLayout = new QVBoxLayout(verticalContainer);
  m_runVButtonLayout->setSpacing(4);
  m_runVButtonLayout->setContentsMargins(0, 0, 0, 0);
  m_runVButtonLayout->setAlignment(Qt::AlignCenter);
  m_runVButtonLayout->addWidget(runButtonV);
  m_runVButtonLayout->addWidget(runStaticButtonV);

  QWidget *horizontalContainer = new QWidget(runButtonWidget);
  m_runHButtonLayout = new QHBoxLayout(horizontalContainer);
  m_runHButtonLayout->setSpacing(4);
  m_runHButtonLayout->setContentsMargins(0, 0, 0, 0);
  m_runHButtonLayout->setAlignment(Qt::AlignCenter);
  m_runHButtonLayout->addWidget(runButtonH);
  m_runHButtonLayout->addWidget(runStaticButtonH);

  stackedLayout->addWidget(horizontalContainer);
  stackedLayout->addWidget(verticalContainer);
  stackedLayout->setCurrentIndex(0);

  connect(m_buttonToggle, &QPushButton::clicked, this,
          [this, stackedLayout, runButtonH, runStaticButtonH]() {
            m_isCollapsed = !m_isCollapsed;
            if (m_isCollapsed) {
              runButtonH->setSize(70, 40);
              runStaticButtonH->setSize(70, 40);
            } else {
              runButtonH->setSize(132, 40);
              runStaticButtonH->setSize(132, 40);
            }

            m_inputPanel->toggleCollapse(m_isCollapsed);
            m_outputPanel->toggleCollapse(m_isCollapsed);
            m_buttonToggle->toggleCollapse();
            stackedLayout->setCurrentIndex(m_isCollapsed ? 1 : 0);
            setFixedWidth(m_isCollapsed ? 160 : 320);
          });
  auto setupDynamicRunHandler = [this](ButtonAction *runBtn,
                                       ButtonAction *staticBtn) {
    return [this, runBtn, staticBtn]() {
      QFuture<void> future = QtConcurrent::run(
          [this]() { m_trainSimulation->simulateDynamicTrainMovement(); });
      updateButtonState(future, runBtn, staticBtn);
    };
  };

  auto setupStaticRunHandler = [this](ButtonAction *runBtn,
                                      ButtonAction *staticBtn) {
    return [this, runBtn, staticBtn]() {
      QFuture<void> future = QtConcurrent::run(
          [this]() { m_trainSimulation->simulateStaticTrainMovement(); });
      updateButtonState(future, runBtn, staticBtn);
    };
  };

  connect(runButtonV, &ButtonAction::clicked, this,
          setupDynamicRunHandler(runButtonV, runStaticButtonV));
  connect(runStaticButtonV, &ButtonAction::clicked, this,
          setupStaticRunHandler(runButtonV, runStaticButtonV));
  connect(runButtonH, &ButtonAction::clicked, this,
          setupDynamicRunHandler(runButtonH, runStaticButtonH));
  connect(runStaticButtonH, &ButtonAction::clicked, this,
          setupStaticRunHandler(runButtonH, runStaticButtonH));

  // QFrame *topSeparator = new QFrame();
  // topSeparator->setFrameShape(QFrame::HLine);
  // topSeparator->setFrameShadow(QFrame::Sunken);
  // topSeparator->setFixedHeight(1);

  // QFrame *bottomSeparator = new QFrame();
  // bottomSeparator->setFrameShape(QFrame::HLine);
  // bottomSeparator->setFrameShadow(QFrame::Sunken);
  // bottomSeparator->setFixedHeight(1);

  // m_buttonLayout->addWidget(topSeparator);
  m_buttonLayout->addWidget(runButtonWidget);
  // m_buttonLayout->addWidget(bottomSeparator);
}

void LeftPanel::setupInputPageButtons() {
  m_inputPanel = new LeftPanelInputs(this, LeftPanelInputs::INPUT);
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
  m_outputPanel = new LeftPanelInputs(this, LeftPanelInputs::OUTPUT);
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
  QTimer::singleShot(200, this, [this, runButton, runStaticButton, future]() {
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this,
            [this, watcher, runStaticButton, runButton]() {
              runStaticButton->setEnabled(true);
              runButton->setEnabled(true);
              watcher->deleteLater();
              QMessageBox::information(this, "Information",
                                       "Simulation completed.");
            });
    watcher->setFuture(future);
  });
}

void LeftPanel::onSimulationCompleted() {
  QMessageBox::information(this, "Simulation", "Simulation completed.");
}