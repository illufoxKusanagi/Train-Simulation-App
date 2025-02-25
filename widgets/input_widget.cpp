#include "input_widget.h"

InputWidget::InputWidget(const InputType &inputType, QWidget *parent)
    : QWidget(parent), m_inputField(nullptr) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  m_label = new QLabel(inputType.label, this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addWidget(m_label);
  m_label->setStyleSheet(TextStyle::BodyMediumRegular() +
                         "color: " + Colors::Secondary700.name() + ";");

  if (inputType.type == "field") {
    m_inputField = new InputField(inputType.unit, this);
    m_inputField->setPlaceholder();
    m_inputField->setReadOnly(inputType.isReadOnly);
    m_inputField->setValue(inputType.value);
    layout->addWidget(m_inputField);
    layout->setAlignment(m_inputField, Qt::AlignLeft);
  } else if (inputType.type == "dropdown") {
    m_inputDropdown = new InputDropdown(this);
    m_inputDropdown->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(m_inputDropdown);
    layout->setAlignment(m_inputDropdown, Qt::AlignLeft);
  } else if (inputType.type == "upload") {
    m_inputUpload = new InputUpload(this);
    m_inputUpload->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(m_inputUpload);
    layout->setAlignment(m_inputUpload, Qt::AlignLeft);
  } else {
    m_inputInvalid = new InputInvalid(inputType.type, this);
    layout->addWidget(m_inputInvalid);
    layout->setAlignment(m_inputInvalid, Qt::AlignLeft);
  }
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  adjustSize();
}

void InputWidget::setValue(double value) {}

void InputWidget::getValue() {}