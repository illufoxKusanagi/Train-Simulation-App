#include "leftPanel.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include "buttonSidebarActive.h"
#include <QDebug>
#include <QVBoxLayout>

LeftPanel::LeftPanel(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->setAlignment(Qt::AlignVCenter);
  setStyleSheet("QWidget {"
                "    background-color: " +
                Colors::Secondary500.name() +
                ";"
                "}");
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
    layout->addWidget(button);
  }
  setLayout(layout);
  // Tombol expand/collapse
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
  layout->addWidget(toggleButton);

  // Hubungkan tombol toggle ke slot togglePanel
  connect(toggleButton, &QPushButton::clicked, this, &LeftPanel::togglePanel);

  // Tetapkan layout
  setLayout(layout);
}

void LeftPanel::togglePanel() {
  // Ganti status collapse/expand
  isCollapsed = !isCollapsed;

  // Ubah ukuran dan teks tombol
  if (isCollapsed) {
    setMaximumWidth(200); // Lebar panel saat collapsed
    toggleButton->setText("Expand");
  } else {
    setMaximumWidth(240); // Lebar panel saat expanded
    toggleButton->setText("Collapse");
  }
  setButtonLabelsVisible(!isCollapsed);
  updateGeometry();
}

void LeftPanel::setButtonLabelsVisible(bool visible) {
  for (ButtonSidebarActive *button : sidebarButtons) {
    button->setLabelVisible(visible);
  }
}
