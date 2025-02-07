#include "left_panel.h"

LeftPanel::LeftPanel(QWidget *parent)
    : QWidget(parent), m_buttonLayout(nullptr), m_inputPanel(nullptr) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(8);
  m_buttonToggle = new ButtonToggle(m_isCollapsed, this);
  QWidget *buttonContainer = new QWidget(this);

  m_buttonLayout = new QVBoxLayout(buttonContainer);
  m_buttonLayout->setContentsMargins(8, 16, 8, 16);
  m_buttonLayout->setSpacing(16);
  m_buttonLayout->setAlignment(Qt::AlignCenter);
  m_buttonLayout->addWidget(m_buttonToggle);
  connect(m_buttonToggle, &QPushButton::clicked, this, [this]() {
    m_isCollapsed = !m_isCollapsed;
    m_inputPanel->toggleCollapse(m_isCollapsed);
    m_outputPanel->toggleCollapse(m_isCollapsed);
    m_buttonToggle->toggleCollapse();
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

// void LeftPanel::setupButtons() {

//   const QStringList buttonNames = {"Constant Values",      "Train Parameter",
//                                    "Running Parameter",    "Track Parameter",
//                                    "Electrical Parameter", "Output"};
//   const QStringList buttonTypes = {"constant", "train",      "running",
//                                    "track",    "electrical", "output"};

//   for (int i = 0; i < buttonNames.size(); ++i) {
//     ButtonSidebarActive *button =
//         new ButtonSidebarActive(buttonTypes[i], buttonNames[i], this);
//     connect(button, &QPushButton::clicked, this, [this, i]() {
//       m_currentIndex = i;
//       updateButtonStates();
//       emitNavigateSignal(i);
//     });
//     sidebarButtons.append(button);
//     buttonLayout->addWidget(button);
//   }
//   updateButtonStates();
// }

// void LeftPanel::updateButtonStates() {
//   for (int i = 0; i < sidebarButtons.size(); ++i) {
//     if (sidebarButtons[i]) {
//       sidebarButtons[i]->setEnabled(i == m_currentIndex);
//     }
//   }
// }

void LeftPanel::emitNavigateSignal(int pageIndex) {
  emit navigateToPage(pageIndex);
}


void LeftPanel::setupInputPageButtons() {
  m_inputPanel = new LeftPanelCategory(LeftPanelCategory::INPUT, this);
  connect(m_inputPanel, &LeftPanelCategory::buttonClicked, this,
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
  m_outputPanel = new LeftPanelCategory(LeftPanelCategory::OUTPUT, this);
  connect(m_outputPanel, &LeftPanelCategory::buttonClicked, this,
          [this](int index) {
            m_currentIndex = index + 5;
            emitNavigateSignal(m_currentIndex);
            m_outputPanel->updateCategoryButton(m_currentIndex);
            m_inputPanel->updateCategoryButton(m_currentIndex);
            m_inputPanel->setCurrentIndex(m_currentIndex);
          });
  m_buttonLayout->addWidget(m_outputPanel);
}
