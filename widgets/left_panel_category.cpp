#include "left_panel_category.h"

LeftPanelInputs::LeftPanelInputs(PanelType type, QWidget *parent)
    : QWidget(parent), m_buttonLayout(new QVBoxLayout(this)), m_type(type),
      local_buttonLayout(nullptr),
      m_buttonTypes(type == INPUT ? INPUT_BUTTON_TYPES : OUTPUT_BUTTON_TYPES),
      m_buttonNames(type == INPUT ? INPUT_BUTTON_NAMES : OUTPUT_BUTTON_NAMES) {
  m_buttonLayout->setContentsMargins(0, 0, 0, 0);
  m_buttonLayout->setSpacing(16);
  m_buttonLayout->setAlignment(Qt::AlignCenter);
  m_categoryButton =
      new ButtonSidebarActive(type == INPUT ? "input" : "input",
                              type == INPUT ? "Inputs" : "Outputs", this);
  m_categoryButton->setEnabled(m_currentIndex == -1);
  m_buttonLayout->addWidget(m_categoryButton);
  // ButtonPanelCategory *categoryLabel = new ButtonPanelCategory(
  //     "category", type == INPUT ? "Inputs" : "Outputs", this);
  // m_buttonLayout->addWidget(categoryLabel);
  setupButtons();
}

void LeftPanelInputs::setupButtons() {
  QWidget *buttonContainer = new QWidget(this);
  local_buttonLayout = new QVBoxLayout(buttonContainer);
  local_buttonLayout->setContentsMargins(16, 0, 0, 0);
  for (int i = 0; i < m_buttonNames.size(); i++) {
    ButtonSidebarActive *button =
        new ButtonSidebarActive(m_buttonTypes[i], m_buttonNames[i], this);
    connect(button, &QPushButton::clicked, this, [this, i]() {
      m_currentIndex = i;
      updateButtonStates();
      emit buttonClicked(i);
    });
    m_sidebarButtons.append(button);
    local_buttonLayout->addWidget(button);
  }
  buttonContainer->setLayout(local_buttonLayout);
  m_buttonLayout->addWidget(buttonContainer);
  updateButtonStates();
}

void LeftPanelInputs::updateButtonStates() {
  // m_categoryLabel->setActive(m_currentIndex == -1, m_currentIndex == 0);
  for (int i = 0; i < m_sidebarButtons.size(); i++) {
    if (m_sidebarButtons[i]) {
      m_sidebarButtons[i]->setEnabled(i == m_currentIndex);
    }
  }
}

void LeftPanelInputs::onPageChanged(int pageIndex) {
  m_currentIndex = pageIndex;
  updateButtonStates();
}

void LeftPanelInputs::toggleCollapse(bool isCollapsed) {
  m_categoryButton->setIconVisible(!isCollapsed);
  m_buttonLayout->setAlignment(isCollapsed ? Qt::AlignCenter : Qt::AlignLeft);
  local_buttonLayout->setContentsMargins(isCollapsed ? 0 : 16, 0, 0, 0);
  for (auto *button : m_sidebarButtons) {
    if (button) {
      button->setLabelVisible(!isCollapsed);
    }
  }
}
const QStringList LeftPanelInputs::INPUT_BUTTON_NAMES = {
    "Constant Values", "Train Parameter", "Running Parameter",
    "Track Parameter", "Electrical Parameter"};

const QStringList LeftPanelInputs::INPUT_BUTTON_TYPES = {
    "constant", "train", "running", "track", "electrical"};

const QStringList LeftPanelInputs::OUTPUT_BUTTON_NAMES = {
    "Train Speed", "Traction Effort", "Train Power"};

const QStringList LeftPanelInputs::OUTPUT_BUTTON_TYPES = {"speed", "traction",
                                                          "power"};
