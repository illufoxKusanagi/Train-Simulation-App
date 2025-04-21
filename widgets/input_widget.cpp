#include "input_widget.h"

InputWidget::InputWidget(QWidget *parent, const InputType &inputType,
                         QStringList options)
    : QWidget(parent), m_inputField(nullptr), m_inputUpload(nullptr),
      m_inputDropdown(nullptr), m_inputInvalid(nullptr) {
  layout = new QVBoxLayout(this);

  m_label = new QLabel(inputType.label, this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addWidget(m_label);
  m_label->setStyleSheet(TextStyle::BodyMediumRegular() +
                         "color: " + Colors::Secondary700.name() +
                         ";"
                         "white-space: normal;");
  m_label->setWordWrap(true);
  m_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  if (inputType.type == "field") {
    buildInputField(inputType);
  } else if (inputType.type == "dropdown") {
    m_inputDropdown = new InputDropdown(this, options);
    connect(m_inputDropdown, &InputDropdown::valueChanged, this, [this]() {
      m_inputValue = m_inputDropdown->getValue();
      emit valueChanged();
    });
    layout->addWidget(m_inputDropdown);
    layout->setAlignment(m_inputDropdown, Qt::AlignLeft);
  } else if (inputType.type == "upload") {
    buildInputUpload(inputType);
  } else {
    m_inputInvalid = new InputInvalid(inputType.type, this);
    layout->addWidget(m_inputInvalid);
    layout->setAlignment(m_inputInvalid, Qt::AlignLeft);
  }
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  adjustSize();
}

void InputWidget::buildInputField(InputType inputType) {
  m_inputField = new InputField(inputType.unit, this);
  m_inputField->setPlaceholder();
  m_inputField->setReadOnly(inputType.isReadOnly);
  m_inputField->setValue(inputType.value);
  m_inputField->connectTextChanged();

  connect(m_inputField->findChild<QLineEdit *>(), &QLineEdit::textChanged, this,
          [this]() {
            m_inputValue = m_inputField->getValue();
            emit valueChanged();
          });
  layout->addWidget(m_inputField);
  layout->setAlignment(m_inputField, Qt::AlignLeft);
}

QList<double> InputWidget::getCsvValue(const int requiredColumn) {
  if (m_inputUpload) {
    return m_inputUpload->getColumnData(requiredColumn);
  }
  return QList<double>();
}

void InputWidget::setValue(double value) {
  if (m_inputField) {
    m_inputValue = value;
    m_inputField->setValue(value);
  } else if (m_inputDropdown) {
    m_inputValue = m_inputDropdown->getValue();
  }
}

double InputWidget::getValue() { return m_inputValue; }

bool InputWidget::isModified() const {
  return m_inputField ? m_inputField->isModified() : false;
}

void InputWidget::setModified(bool modified) {
  if (m_inputField) {
    m_inputField->setModified(modified);
  }
}

void InputWidget::buildInputUpload(InputType inputType) {
  m_inputUpload = new InputUpload(this);
  m_inputUpload->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  m_inputUpload->setRequiredColumnCount(3);
  m_inputUpload->setTargetColumns({0, 1, 2});
  connect(m_inputUpload, &InputUpload::fileLoaded, this,
          [this]() { emit valueChanged(); });
  connect(m_inputUpload, &InputUpload::fileLoaded, this, [this]() {
    if (m_inputUpload->hasRequiredColumns()) {
      QList<double> speedData = m_inputUpload->getColumnData(0);
    }
  });
  connect(m_inputUpload, &InputUpload::fileLoadError, this,
          [this](const QString &errorMessage) {
            MessageBoxWidget messageBox("CSV Load Error", errorMessage,
                                        MessageBoxWidget::Warning);
          });
  layout->addWidget(m_inputUpload);
  layout->setAlignment(m_inputUpload, Qt::AlignLeft);
}