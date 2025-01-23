#include "buttonAction.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include <qboxlayout.h>
#include <qstyle.h>

ButtonAction::ButtonAction(const QString &buttonText,
                           const QString &isRunButton, QWidget *parent)
    : QPushButton(buttonText, parent) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  if (isRunButton == "icon") {
    setIcon(QIcon(":/icons/icons/right-arrow-active.svg"));
    setIconSize(QSize(24, 24));
  }
  setStyleSheet("QPushButton {"
                "    padding: 12px 4px;"
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

QString ButtonAction::getText() const { return buttonAction->text(); }
