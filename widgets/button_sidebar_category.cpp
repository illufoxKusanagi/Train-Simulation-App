#include "button_sidebar_category.h"

ButtonSidebarCategory::ButtonSidebarCategory(const QString &iconType,
                                             const QString &buttonLabel,
                                             QWidget *parent)
    : ButtonSidebarActive(iconType, buttonLabel, parent) {}

void ButtonSidebarCategory::setActive(bool isCollapsed, bool isActive) {
  m_iconType = !isCollapsed ? m_iconType + "" : m_iconType + "Collapse";
  ButtonSidebarActive::setEnabled(isActive);
  //   QString iconPath =
  //       IconPaths::getIconPath(isActive ? m_iconType : m_iconType +
  //       "Inactive");
  //   QPixmap icon(iconPath);
  //   m_iconLabel->setPixmap(
  //       icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
