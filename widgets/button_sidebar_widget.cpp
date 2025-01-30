#include "button_sidebar_widget.h"
#include "../resources/icon_paths.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QSizePolicy>

ButtonSidebarWidget::ButtonSidebarWidget(const QString &iconType,
                                         const QString &buttonLabel,
                                         QWidget *parent)
    : QWidget(parent) {
  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(16);
  iconLabel = new QLabel(this);
  QString iconPath = IconPaths::getIconPath(iconType);
  QPixmap icon(iconPath);
  iconLabel->setPixmap(
      icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  iconLabel->setFixedSize(40, 40);
  iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  textLabel = new QLabel(buttonLabel, this);
  textLabel->setStyleSheet(TextStyle::BodyBigBold() +
                           "color: " + Colors::StandardWhite.name() + ";");
  textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layout->addWidget(iconLabel);
  layout->addWidget(textLabel, 1);
  setLayout(layout);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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

void ButtonSidebarWidget::updateIcon(const QString &iconPath) {
  if (iconLabel) {
    iconLabel->setPixmap(QPixmap(iconPath));
  }
}
