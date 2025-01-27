#include "button_action.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
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
                "    max-width: 150px;"
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
                "}");
}

QString ButtonAction::getText() const { return buttonAction->text(); }
