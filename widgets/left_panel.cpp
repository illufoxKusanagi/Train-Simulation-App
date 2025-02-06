#include "left_panel.h"

LeftPanel::LeftPanel(QWidget *parent)
    : QWidget(parent), buttonLayout(nullptr), m_inputPanel(nullptr) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(8);
  buttonToggle = new ButtonToggle(m_isCollapsed, this);
  QWidget *buttonContainer = new QWidget(this);

  buttonLayout = new QVBoxLayout(buttonContainer);
  buttonLayout->setContentsMargins(8, 16, 8, 16);
  buttonLayout->setSpacing(16);
  buttonLayout->setAlignment(Qt::AlignCenter);
  buttonLayout->addWidget(buttonToggle);
  connect(buttonToggle, &QPushButton::clicked, this, [this]() {
    m_isCollapsed = !m_isCollapsed;
    m_inputPanel->toggleCollapse(m_isCollapsed);
    m_outputPanel->toggleCollapse(m_isCollapsed);
    buttonToggle->toggleCollapse();
    setFixedWidth(m_isCollapsed ? 120 : 320);
  });

  setupInputPageButtons();
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

void LeftPanel::setupInputPageButtons() {
  m_inputPanel = new LeftPanelInputs(LeftPanelInputs::INPUT, this);
  connect(m_inputPanel, &LeftPanelInputs::buttonClicked, this,
          [this](int index) {
            m_currentIndex = index;
            emitNavigateSignal(index);
            m_inputPanel->updateCategoryButton(m_currentIndex);
            m_outputPanel->updateCategoryButton(m_currentIndex);
            m_outputPanel->setCurrentIndex(m_currentIndex - m_currentIndex - 1);
            qDebug() << "Left Panel input Current Index: " << m_currentIndex;
          });
  buttonLayout->addWidget(m_inputPanel);
}

void LeftPanel::setupOutputPageButtons() {
  m_outputPanel = new LeftPanelInputs(LeftPanelInputs::OUTPUT, this);
  connect(m_outputPanel, &LeftPanelInputs::buttonClicked, this,
          [this](int index) {
            m_currentIndex = index + 5;
            qDebug() << "Left Panel output Current Index: " << m_currentIndex;
            emitNavigateSignal(m_currentIndex);
            m_outputPanel->updateCategoryButton(m_currentIndex);
            m_inputPanel->updateCategoryButton(m_currentIndex);
            m_inputPanel->setCurrentIndex(m_currentIndex);
          });
  buttonLayout->addWidget(m_outputPanel);
}
