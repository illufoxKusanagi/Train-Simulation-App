#include "button_sidebar_widget.h"

ButtonSidebarWidget::ButtonSidebarWidget(const QString &iconType,
                                         const QString &buttonLabel,
                                         QWidget *parent)
    : QWidget(parent) {
  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(16);
  iconLabel = new QLabel(this);
  m_iconType = iconType;
  updateIcon(m_isEnabled);
  iconLabel->setFixedSize(40, 40);
  iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  textLabel = new QLabel(buttonLabel, this);
  textLabel->setStyleSheet(TextStyle::BodyBigBold() +
                           "color: " + Colors::StandardWhite.name() + ";");
  layout->addWidget(iconLabel);
  layout->addWidget(textLabel, 1);
  setLayout(layout);
  setMinimumHeight(40);
  setMaximumHeight(40);
  setStyleSheet("QWidget { background-color: transparent; }");
}

void ButtonSidebarWidget::setLabelVisible(bool visible) {
  if (textLabel) {
    textLabel->setVisible(visible);
    if (layout) {
      layout->setSpacing(visible ? 16 : 0);
    }
  }
}

void ButtonSidebarWidget::setIconVisible(bool visible) {
  if (iconLabel) {
    iconLabel->setVisible(visible);
    if (layout) {
      layout->setSpacing(visible ? 16 : 0);
      layout->setAlignment(textLabel,
                           visible ? Qt::AlignLeft : Qt::AlignCenter);
    }
  }
}

void ButtonSidebarWidget::updateIcon(bool isEnabled) {
  QString iconPath = IconPaths::getIconPath(
      m_isEnabled ? m_iconType : m_iconType + "Disabled");
  QPixmap icon(iconPath);
  iconLabel->setPixmap(
      icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ButtonSidebarWidget::setEnabled(bool isEnabled) {
  textLabel->setStyleSheet(
      TextStyle::BodyBigBold() + "color: " +
      (isEnabled ? Colors::StandardWhite : Colors::Grey400).name() + ";");
  m_isEnabled = isEnabled;
  updateIcon(m_isEnabled);
}
