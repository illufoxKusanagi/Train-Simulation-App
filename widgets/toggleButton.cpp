#include "toggleButton.h"
#include "../styles/colors.h"

ToggleButton::ToggleButton(bool isCollapsed, QWidget *parent)
    : QPushButton(parent) {
  updateIcon();
  setStyleSheet("QPushButton {"
                "    padding: 8px;"
                "    color: white;"
                "    background-color: transparent;"
                "    border: none;"
                "    border-radius: 4px;"
                "}"
                "QPushButton:hover {"
                "    background-color: " +
                Colors::Secondary300.name() +
                ";"
                "}");
}

void ToggleButton::updateIcon() {
  QString iconPath =
      m_isCollapsed ? ":/icons/icons/collapse.svg" : ":/icons/icons/expand.svg";
  setIcon(QIcon(iconPath));
}

void ToggleButton::toggleCollapse() {
  m_isCollapsed = !m_isCollapsed;
  updateIcon();
  qDebug() << "Icon updated";
}
