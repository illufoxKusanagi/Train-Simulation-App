#include "buttonSidebarWidget.h"
#include "../resources/iconPaths.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include <QPixmap>
#include <QSizePolicy>

ButtonSidebarWidget::ButtonSidebarWidget(const QString &iconType,
                                         const QString &buttonLabel,
                                         QWidget *parent)
    : QWidget(parent) {
  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
  layout->setContentsMargins(0, 0, 0, 0);
  iconLabel = new QLabel(this);
  QString iconPath = IconPaths::getIconPath(iconType);
  QPixmap icon(iconPath);
  iconLabel->setPixmap(
      icon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  iconLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  QLabel *textLabel = new QLabel(buttonLabel, this);
  textLabel->setStyleSheet(TextStyle::BodyBigBold() +
                           "color: " + Colors::StandardWhite.name() + ";");
  textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  layout->addWidget(iconLabel);
  layout->setSpacing(16);
  layout->addWidget(textLabel);
  setLayout(layout);
  setStyleSheet("QWidget {"
                "    background-color: transparent;"
                "}");
}

void ButtonSidebarWidget::setLabelVisible(bool visible) {
  textLabel->setVisible(visible);
  if (visible) {
    layout->setSpacing(16);
  } else {
    layout->setSpacing(0);
  }
}
