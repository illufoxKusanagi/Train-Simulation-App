#include "button_panel_category.h"

ButtonPanelCategory::ButtonPanelCategory(const QString &iconType,
                                         const QString &buttonLabel,
                                         QWidget *parent)
    : QPushButton(parent), m_buttonPanelCategory(nullptr), mainLayout(nullptr),
      m_iconLabel(nullptr), m_textLabel(nullptr), m_iconType(iconType),
      m_buttonLabel(buttonLabel) {
  mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
  mainLayout->setContentsMargins(8, 8, 8, 8);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setFixedHeight(40);
  m_buttonPanelCategory->setSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed);
  setTextIcon();
}

void ButtonPanelCategory::setActive(bool isActive) {
  m_isActive = isActive;
  m_textLabel->setStyleSheet(
      "background: transparent;" + TextStyle::BodyBigBold() + "color: " +
      (isActive ? Colors::StandardWhite : Colors::Grey400).name() + ";");
  updateIcon(isActive);
}

void ButtonPanelCategory::setTextIcon() {
  QWidget *iconWidget = new QWidget(this);
  QHBoxLayout *iconLayout = new QHBoxLayout(iconWidget);
  iconLayout->setContentsMargins(0, 0, 0, 0);
  m_iconLabel = new QLabel(this);
  m_textLabel = new QLabel(m_buttonLabel, this);
  iconLayout->addWidget(m_iconLabel);
  iconLayout->addWidget(m_textLabel, 1);
  iconWidget->setLayout(iconLayout);
  iconWidget->setStyleSheet("QWidget { background-color: transparent; }");
  mainLayout->addWidget(iconWidget);
}

void ButtonPanelCategory::updateIcon(bool isActive) {
  QString iconPath =
      IconPaths::getIconPath(isActive ? m_iconType : m_iconType + "Inactive");
  QPixmap icon(iconPath);
  m_iconLabel->setPixmap(
      icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

const QString ButtonPanelCategory::s_styleBase =
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

const QString ButtonPanelCategory::s_styleHover = "QPushButton:hover {"
                                                  "    background-color: " +
                                                  Colors::Secondary300.name() +
                                                  ";"
                                                  "}";

const QString ButtonPanelCategory::s_stylePressed =
    "QPushButton:pressed {"
    "    background-color: " +
    Colors::Secondary600.name() +
    ";"
    "}";
