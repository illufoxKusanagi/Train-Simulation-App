#include "input_widget.h"

InputWidget::InputWidget(const InputType &inputType, QWidget *parent)
    : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  QWidget *inputField = nullptr;
  label = new QLabel(inputType.label, this);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addWidget(label);
  label->setStyleSheet(TextStyle::BodyMediumRegular() +
                       "color: " + Colors::Secondary700.name() + ";");

  if (inputType.type == "field") {
    inputField = new InputField(inputType.unit, this);
  } else if (inputType.type == "dropdown") {
    inputField = new InputDropdown(this);
    inputField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  } else if (inputType.type == "upload") {
    inputField = new InputUpload(this);
    inputField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  } else {
    inputField = new InputInvalid(inputType.type, this);
  }
  if (inputField != nullptr) {
    layout->addWidget(inputField);
    layout->setAlignment(inputField, Qt::AlignLeft);
  }
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  adjustSize();
}
