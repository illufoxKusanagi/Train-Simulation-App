#include "buttonSidebar.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include <qboxlayout.h>
#include <qstyle.h>

ButtonSidebar::ButtonSidebar(const QString &iconType,
                             const QString &buttonLabel, QWidget *parent)
    : QPushButton(buttonLabel, parent) {
  setStyleSheet("QPushButton {"
                "    padding: 12px 8px;"
                "    text-align: left;"
                "    border: 0 solid;"
                "    border-radius: 12px;"
                "    background-color: " +
                Colors::Primary600.name() +
                ";"
                "    color: " +
                Colors::StandardWhite.name() +
                ";"
                "    " +
                TextStyle::BodyMediumBold() +
                "}"
                "QPushButton:hover {"
                "    background-color: " +
                Colors::Primary500.name() +
                ";"
                "}"
                "QPushButton:pressed {"
                "    background-color: " +
                Colors::Primary600.name() +
                ";"
                "}"
                "QPushButton:disabled {"
                "    border-color: " +
                Colors::Grey400.name() +
                ";"
                "    color: " +
                Colors::Grey400.name() +
                ";"
                "}"
                "QPushButton:before {"
                "    content: '';"
                "    position: absolute;"
                "    left: 10px;"
                "    top: 50%;"
                "    transform: translateY(-50%);"
                "    width: 24px;"
                "    height: 24px;"
                "    background-image: url(:/icons/icons/nav-arrow-down.png);"
                "    background-size: contain;"
                "    background-repeat: no-repeat;"
                "}");
}
