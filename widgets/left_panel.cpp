#include "left_panel.h"

LeftPanel::LeftPanel(QWidget *parent) : QWidget(parent), buttonLayout(nullptr) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(8);
  // QVBoxLayout *buttonLayout new QVBoxLayout(this);
  // mainLayout->setAlignment(Qt::AlignVCenter);
  buttonToggle = new ButtonToggle(isCollapsed, this);
  QWidget *buttonContainer = new QWidget(this);

  buttonLayout = new QVBoxLayout(buttonContainer);
  buttonLayout->setContentsMargins(8, 16, 8, 16);
  buttonLayout->setSpacing(16);
  buttonLayout->setAlignment(Qt::AlignCenter);
  buttonLayout->addWidget(buttonToggle);
  connect(buttonToggle, &QPushButton::clicked, this, [this]() {
    isCollapsed = !isCollapsed;
    for (auto *button : sidebarButtons) {
      if (button) {
        button->setLabelVisible(!isCollapsed);
      }
    }
    buttonToggle->toggleCollapse();
    setFixedWidth(isCollapsed ? 80 : 320);
  });

  setupButtons();
  mainLayout->addWidget(buttonContainer);
  setFixedWidth(320);
  setLayout(mainLayout);
  setStyleSheet("QWidget {"
                "    background-color: " +
                Colors::Secondary500.name() +
                ";"
                "}");
}

void LeftPanel::setupButtons() {
  const QStringList buttonNames = {"Constant Values",      "Train Parameter",
                                   "Running Parameter",    "Track Parameter",
                                   "Electrical Parameter", "Output"};
  const QStringList buttonTypes = {"constantActive",   "trainActive",
                                   "runningActive",    "trackActive",
                                   "electricalActive", "outputActive"};

  for (int i = 0; i < buttonNames.size(); ++i) {
    ButtonSidebarActive *button =
        new ButtonSidebarActive(buttonTypes[i], buttonNames[i], this);
    connect(button, &QPushButton::clicked, this,
            [this, i]() { emitNavigateSignal(i); });
    sidebarButtons.append(button);
    buttonLayout->addWidget(button);
  }
  // setSidebarButtonsEnabled(false);
}

// void LeftPanel::setSidebarButtonsEnabled(bool enable) {
//   for (auto *button : sidebarButtons) {
//     if (button) {
//       button->setEnabled(enable);
//     }
//   }
// }

void LeftPanel::emitNavigateSignal(int pageIndex) {
  emit navigateToPage(pageIndex);
}

// void LeftPanel::onPageChanged(int pageIndex) {
//   // For example, only enable the buttons if we’re past page 0
//   setSidebarButtonsEnabled(pageIndex > 0);
// }
