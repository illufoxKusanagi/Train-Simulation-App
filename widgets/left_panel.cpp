#include "left_panel.h"

LeftPanel::LeftPanel(QWidget *parent)
    : QWidget(parent), m_buttonLayout(nullptr), m_inputPanel(nullptr) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  m_buttonToggle = new ButtonToggle(m_isCollapsed, this);

  QWidget *buttonContainer = new QWidget(this);
  m_buttonLayout = new QVBoxLayout(buttonContainer);
  m_buttonLayout->setContentsMargins(8, 16, 8, 16);
  m_buttonLayout->setSpacing(8);
  m_buttonLayout->setAlignment(Qt::AlignCenter);
  m_buttonLayout->addWidget(m_buttonToggle);

  connect(m_buttonToggle, &QPushButton::clicked, this, [this]() {
    m_isCollapsed = !m_isCollapsed;
    m_inputPanel->toggleCollapse(m_isCollapsed);
    m_outputPanel->toggleCollapse(m_isCollapsed);
    m_buttonToggle->toggleCollapse();
    setFixedWidth(m_isCollapsed ? 160 : 320);
  });

  setupInputPageButtons();
  createRunButton();
  setupOutputPageButtons();
  mainLayout->addWidget(buttonContainer);
  setFixedWidth(320);
  setLayout(mainLayout);
  setStyleSheet("QWidget {"
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
  QHBoxLayout *runButtonLayout = new QHBoxLayout(runButtonWidget);
  ButtonAction *runButton = new ButtonAction("Run", "yes", this);
  connect(runButton, &ButtonAction::clicked, this,
          [this]() { emitNavigateSignal(m_currentIndex); });
  runButton->setEnabled(true);
  runButton->setSize(80, 40);
  runButtonLayout->setContentsMargins(0, 0, 0, 0);
  runButtonLayout->setAlignment(Qt::AlignCenter);
  runButtonLayout->addWidget(runButton);
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
