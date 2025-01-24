#include "leftPanel.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include "buttonSidebarActive.h"
#include "toggleButton.h"
#include <QVBoxLayout>

LeftPanel::LeftPanel(QWidget *parent) : QWidget(parent), buttonLayout(nullptr) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  setStyleSheet("QWidget {"
                "    background-color: " +
                Colors::Secondary500.name() +
                ";"
                "}");
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(8);
  mainLayout->setAlignment(Qt::AlignVCenter);

  toggleButton = new ToggleButton(isCollapsed, this);
  QWidget *buttonContainer = new QWidget(this);
  buttonLayout = new QVBoxLayout(buttonContainer);
  buttonLayout->setContentsMargins(8, 16, 8, 16);
  buttonLayout->setSpacing(16);
  buttonLayout->setAlignment(Qt::AlignCenter);
  buttonLayout->addWidget(toggleButton);
  connect(toggleButton, &QPushButton::clicked, this, [this]() {
    isCollapsed = !isCollapsed;
    for (auto *button : sidebarButtons) {
      if (button) {
        button->setLabelVisible(!isCollapsed);
      }
    }
    toggleButton->toggleCollapse();
    setFixedWidth(isCollapsed ? 80 : 320);
  });

  setupButtons();
  mainLayout->addWidget(buttonContainer);
  setFixedWidth(320);
  setLayout(mainLayout);
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
}

void LeftPanel::emitNavigateSignal(int pageIndex) {
  emit navigateToPage(pageIndex);
}
