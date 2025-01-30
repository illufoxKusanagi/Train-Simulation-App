#include "button_action.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include <QHBoxLayout>
#include <QStyle>

ButtonAction::ButtonAction(const QString &buttonText,
                           const QString &isRunButton, QWidget *parent)
    : QPushButton(buttonText, parent) {
  // Set up layout or other initialization...
  QHBoxLayout *layout = new QHBoxLayout(this);
  if (isRunButton == "yes") {
    setIcon(QIcon(":/icons/icons/right-arrow-active.svg"));
    setIconSize(QSize(24, 24));
  }
}

QString ButtonAction::getText() const { return text(); }

void ButtonAction::setEnabled(bool isEnabled) {
  // Call the base class to avoid recursion
  QPushButton::setEnabled(isEnabled);

  // Use different styles, depending on the enabled state
  if (isEnabled) {
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
                  "}");
  } else {
    setStyleSheet("QPushButton {"
                  "    padding: 12px 4px;"
                  "    border: 0 solid;"
                  "    border-radius: 12px;"
                  "    max-width: 150px;"
                  "    background-color: " +
                  Colors::Grey400.name() +
                  ";"
                  "    color: " +
                  Colors::StandardWhite.name() +
                  ";"
                  "    " +
                  TextStyle::BodyMediumBold() + "}");
  }
}