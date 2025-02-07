#include "input_widget.h"

InputWidget::InputWidget(const InputType &inputType, QWidget *parent)
    : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  m_label = new QLabel(inputType.label, this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addWidget(m_label);
  m_label->setStyleSheet(TextStyle::BodyMediumRegular() +
                         "color: " + Colors::Secondary700.name() + ";");

  if (inputType.type == "field") {
    m_inputField = new InputField(inputType.unit, this);
  } else if (inputType.type == "dropdown") {
    m_inputField = new InputDropdown(this);
    m_inputField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  } else if (inputType.type == "upload") {
    m_inputField = new InputUpload(this);
    m_inputField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  } else {
    m_inputField = new InputInvalid(inputType.type, this);
  }
  if (m_inputField != nullptr) {
    layout->addWidget(m_inputField);
    layout->setAlignment(m_inputField, Qt::AlignLeft);
  }
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  adjustSize();
}
