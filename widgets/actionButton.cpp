#include "actionButton.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include <qboxlayout.h>

ActionButton::ActionButton(const QString &buttonText, QWidget *parent)
    : QPushButton(buttonText, parent) {
  QHBoxLayout *layout = new QHBoxLayout(this);
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
                "}");
}

// In actionButton.cpp
QString ActionButton::getText() const { return actionButton->text(); }
// QPushButton* ActionButton::getButton() { return *actionButton; }
