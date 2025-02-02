
#include "input_field.h"

InputField::InputField(const QString &unitText, QWidget *parent)
    : QWidget(parent), input(new QLineEdit(this)),
      unitLabel(new QLabel(unitText, this)) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(input);
  layout->addWidget(unitLabel);

  input->setStyleSheet("min-width: 32px; max-width: 64px; padding: 4px 12px; "
                       "border: 1px solid " +
                       Colors::Secondary400.name() + "; border-radius: 8px;" +
                       TextStyle::BodySmallRegular() +
                       "color: " + Colors::Secondary400.name() + ";");
  unitLabel->setStyleSheet(TextStyle::BodySmallRegular() +
                           "color: " + Colors::Secondary700.name() + ";");
}

QString InputField::text() const { return input->text(); }

void InputField::setText(const QString &text) { input->setText(text); }

void InputField::setSize(int width, int height) {
  input->setFixedSize(width, height);
}