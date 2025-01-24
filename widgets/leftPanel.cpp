#include "leftPanel.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include "buttonSidebarActive.h"
#include <QDebug>
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

  toggleButton = new QPushButton("Collapse", this);
  toggleButton->setStyleSheet("QPushButton {"
                              "    padding: 8px;"
                              "    color: white;"
                              "    background-color: " +
                              Colors::Secondary600.name() +
                              ";"
                              "    border: none;"
                              "    border-radius: 4px;"
                              "}"
                              "QPushButton:hover {"
                              "    background-color: " +
                              Colors::Secondary300.name() +
                              ";"
                              "}");
  mainLayout->addWidget(toggleButton);
  connect(toggleButton, &QPushButton::clicked, this, &LeftPanel::togglePanel);

  QWidget *buttonContainer = new QWidget(this);
  buttonLayout = new QVBoxLayout(buttonContainer);
  buttonLayout->setContentsMargins(8, 16, 8, 16);
  buttonLayout->setSpacing(16);
  buttonLayout->setAlignment(Qt::AlignCenter);
  sidebarButtons.append(
      new ButtonSidebarActive("constantActive", "Constant Values", this));
  sidebarButtons.append(
      new ButtonSidebarActive("trainActive", "Train Parameter", this));
  sidebarButtons.append(
      new ButtonSidebarActive("runningActive", "Running Parameter", this));
  sidebarButtons.append(
      new ButtonSidebarActive("trackActive", "Track Parameter", this));
  sidebarButtons.append(new ButtonSidebarActive("electricalActive",
                                                "Electrical Parameter", this));
  sidebarButtons.append(
      new ButtonSidebarActive("outputActive", "Output", this));
  for (ButtonSidebarActive *button : sidebarButtons) {
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    buttonLayout->addWidget(button);
  }
  mainLayout->addWidget(buttonContainer);
  setFixedWidth(320);
  setLayout(mainLayout);
}

void LeftPanel::togglePanel() {
  isCollapsed = !isCollapsed;
  if (toggleButton) {
    toggleButton->setText(isCollapsed ? "Expand" : "Collapse");
    qDebug() << "Toggle button text set to : " << isCollapsed;
  }
  for (auto *button : sidebarButtons) {
    if (button) {
      button->setLabelVisible(!isCollapsed);
    }
  }
  setFixedWidth(isCollapsed ? 80 : 320);
  if (layout()) {
    layout()->invalidate();
    layout()->update();
  }
  updateGeometry();
}

void LeftPanel::setButtonLabelsVisible(bool visible) {
  for (ButtonSidebarActive *button : sidebarButtons) {
    button->setLabelVisible(visible);
  }
}
