
#include "input_field.h"

InputField::InputField(const QString &unitText, QWidget *parent)
    : QWidget(parent), m_input(new QLineEdit(this)),
      m_unitLabel(new QLabel(unitText, this)) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(m_input);
  layout->addWidget(m_unitLabel);

  m_input->setStyleSheet("min-width: 32px; max-width: 64px; padding: 4px 12px; "
                         "border: 1px solid " +
                         Colors::Secondary400.name() + "; border-radius: 8px;" +
                         TextStyle::BodySmallRegular() +
                         "color: " + Colors::Secondary400.name() + ";");
  m_unitLabel->setStyleSheet(TextStyle::BodySmallRegular() +
                             "color: " + Colors::Secondary700.name() + ";");
}

QString InputField::text() const { return m_input->text(); }