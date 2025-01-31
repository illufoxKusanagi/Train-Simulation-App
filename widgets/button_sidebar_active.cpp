#include "button_sidebar_active.h"

ButtonSidebarActive::ButtonSidebarActive(const QString &iconType,
                                         const QString &buttonLabel,
                                         QWidget *parent)
    : QPushButton(parent), m_contentWidget(nullptr) {
  m_contentWidget = new ButtonSidebarWidget(iconType, buttonLabel, this);
  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
  layout->setContentsMargins(8, 8, 8, 8);
  QString iconPath = IconPaths::getIconPath(iconType);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layout->addWidget(m_contentWidget);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setStyleSheet("QPushButton {"
                "    padding: 8px 24px;"
                "    text-align: left;"
                "    border: 0 solid;"
                "    border-radius: 12px;"
                "    background-color: transparent;"
                "    min-height: 40px;"
                "    color: " +
                Colors::StandardWhite.name() +
                ";"
                "    " +
                TextStyle::BodyBigBold() +
                "}"
                "QPushButton:hover {"
                "    background-color: " +
                Colors::Secondary300.name() +
                ";"
                "}"
                "QPushButton:pressed {"
                "    background-color: " +
                Colors::Secondary600.name() +
                ";"
                "}"
                "QPushButton:disabled {"
                "    border-color: " +
                Colors::Grey400.name() +
                ";"
                "    color: " +
                Colors::Grey400.name() +
                ";"
                "}");
}

void ButtonSidebarActive::setLabelVisible(bool visible) {
  if (m_contentWidget) {
    m_contentWidget->setLabelVisible(visible);
  }
}
