#include "toggle_button.h"
#include "../styles/colors.h"

ToggleButton::ToggleButton(bool isCollapsed, QWidget *parent)
    : QPushButton(parent) {
  updateIcon();
  setIconSize(QSize(36, 36));
  setFixedSize(64, 56);
  setContentsMargins(16, 16, 16, 16);
  setStyleSheet("QPushButton {"
                "    padding: 32px;"
                "    background-color: transparent;"
                "    border: none;"
                "    border-radius: 8px;"
                "    max-size: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: " +
                Colors::Secondary400.name() +
                ";"
                "}");
}

void ToggleButton::updateIcon() {
  QString iconPath =
      m_isCollapsed ? ":/icons/icons/expand.svg" : ":/icons/icons/collapse.svg";
  setIcon(QIcon(iconPath));
}

void ToggleButton::toggleCollapse() {
  m_isCollapsed = !m_isCollapsed;
  updateIcon();
}
