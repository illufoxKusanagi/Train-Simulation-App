#include "buttonSidebarActive.h"
#include "../resources/iconPaths.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include <qboxlayout.h>
#include <qstyle.h>

ButtonSidebarActive::ButtonSidebarActive(const QString &iconType,
                                         const QString &buttonLabel,
                                         QWidget *parent)
    : QPushButton(buttonLabel, parent) {
  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
  QString iconPath = IconPaths::getIconPath(iconType);
  setIcon(QIcon(iconPath));
  setIconSize(QSize(40, 40));
  setStyleSheet("QPushButton {"
                "    padding: 8px 8px;"
                "    text-align: left;"
                "    border: 0 solid;"
                "    border-radius: 12px;"
                "    background-color: transparent;"
                "    min-width: 240px;"
                "    max-width: 240px;"
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
