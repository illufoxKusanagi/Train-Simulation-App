#include "button_sidebar_active.h"

ButtonSidebarActive::ButtonSidebarActive(const QString &iconType,
                                         const QString &buttonLabel,
                                         QWidget *parent)
    : QPushButton(parent), m_contentWidget(nullptr) {
  m_contentWidget = new ButtonSidebarWidget(iconType, buttonLabel, this);
  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
  layout->setContentsMargins(8, 8, 8, 8);
  updateIcon();
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layout->addWidget(m_contentWidget);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  // setStyleSheet("QPushButton {"
  //               "    padding: 8px 24px;"
  //               "    text-align: left;"
  //               "    border: 0 solid;"
  //               "    border-radius: 12px;"
  //               "    background-color: transparent;"
  //               "    min-height: 40px;"
  //               "    color: " +
  //               Colors::StandardWhite.name() +
  //               ";"
  //               "    " +
  //               TextStyle::BodyBigBold() +
  //               "}"
  //               "QPushButton:hover {"
  //               "    background-color: " +
  //               Colors::Secondary300.name() +
  //               ";"
  //               "}"
  //               "QPushButton:pressed {"
  //               "    background-color: " +
  //               Colors::Secondary600.name() +
  //               ";"
  //               "}"
  //               "QPushButton:disabled {"
  //               "    border-color: " +
  //               Colors::Grey400.name() +
  //               ";"
  //               "    color: " +
  //               Colors::Grey400.name() +
  //               ";"
  //               "}");
}

void ButtonSidebarActive::setLabelVisible(bool visible) {
  if (m_contentWidget) {
    m_iconPath = IconPaths::getIconPath(m_iconType);
  }
}

void ButtonSidebarActive::setEnabled(bool isEnabled) {
  m_iconPath =
      IconPaths::getIconPath(isEnabled ? m_iconType : m_iconType + "Disabled");
  setStyleSheet(isEnabled ? s_styleBase : s_styleDisabled);
  updateIcon();
}

void ButtonSidebarActive::updateIcon() {
  m_contentWidget->updateIcon(m_iconPath);
}

const QString ButtonSidebarActive::s_styleHover = "QPushButton:hover {"
                                                  "    background-color: " +
                                                  Colors::Secondary300.name() +
                                                  ";"
                                                  "}";

const QString ButtonSidebarActive::s_stylePressed =
    "QPushButton:pressed {"
    "    background-color: " +
    Colors::Secondary600.name() +
    ";"
    "}";

const QString ButtonSidebarActive::s_styleBase =
    "QPushButton {"
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
    TextStyle::BodyBigBold() + "}";

const QString ButtonSidebarActive::s_styleDisabled =
    "QPushButton {"
    "    padding: 8px 24px;"
    "    text-align: left;"
    "    border: 0 solid;"
    "    border-radius: 12px;"
    "    background-color: transparent;"
    "    min-height: 40px;"
    "    color: " +
    Colors::Grey400.name() +
    ";"
    "    " +
    TextStyle::BodyBigBold() + "}";