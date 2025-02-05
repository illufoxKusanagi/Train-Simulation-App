#include "left_panel_category.h"

LeftPanelInputs::LeftPanelInputs(PanelType type, QWidget *parent)
    : QWidget(parent), buttonLayout(new QVBoxLayout(this)), m_type(type),
      m_buttonTypes(type == INPUT ? INPUT_BUTTON_TYPES : OUTPUT_BUTTON_TYPES),
      m_buttonNames(type == INPUT ? INPUT_BUTTON_NAMES : OUTPUT_BUTTON_NAMES) {
  // buttonLayout->setContentsMargins(8, 16, 8, 16);
  buttonLayout->setContentsMargins(0, 0, 0, 0);
  buttonLayout->setSpacing(16);
  buttonLayout->setAlignment(Qt::AlignCenter);
  // ButtonPanelCategory *categoryLabel = new ButtonPanelCategory(
  //     "category", type == INPUT ? "Inputs" : "Outputs", this);
  // buttonLayout->addWidget(categoryLabel);
  setupButtons();
}

void LeftPanelInputs::setupButtons() {
  for (int i = 0; i < m_buttonNames.size(); i++) {
    ButtonSidebarActive *button =
        new ButtonSidebarActive(m_buttonTypes[i], m_buttonNames[i], this);
    connect(button, &QPushButton::clicked, this, [this, i]() {
      m_currentIndex = i;
      updateButtonStates();
      emit buttonClicked(i);
    });
    m_sidebarButtons.append(button);
    buttonLayout->addWidget(button);
  }
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
  // m_categoryLabel->setLabelVisible(!isCollapsed);
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

const QStringList LeftPanelInputs::OUTPUT_BUTTON_TYPES = {"output", "track",
                                                          "electrical"};
