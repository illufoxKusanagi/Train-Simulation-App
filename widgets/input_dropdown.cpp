#include "input_dropdown.h"

InputDropdown::InputDropdown(QWidget *parent)
    : QWidget(parent), m_dropdown(new QComboBox(this)) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  QStringList options = {"12", "10", "8", "6"};
  layout->setContentsMargins(0, 0, 0, 0);
  m_dropdown->addItems(options);
  m_dropdown->setStyleSheet("QComboBox {"
                            "    padding: 4px 8px;"
                            "    border: 1px solid " +
                            Colors::Secondary400.name() +
                            ";"
                            "    border-radius: 8px;"
                            "    min-width: 100px;" +
                            TextStyle::BodySmallRegular() +
                            "    color: " + Colors::Secondary400.name() +
                            ";"
                            "}"
                            "QComboBox::drop-down {"
                            "    subcontrol-origin: padding;"
                            "    subcontrol-position: center right;"
                            "    width: 10px;"
                            "    height: 10px;"
                            "    border: none;"
                            "}"
                            "QComboBox::down-arrow {"
                            "    image: url(:/icons/icons/expand.svg);"
                            "    width: 8px;"
                            "    height: 8px;"
                            "}");

  layout->addWidget(m_dropdown);
}

QString InputDropdown::currentText() const { return m_dropdown->currentText(); }

void InputDropdown::setCurrentText(const QString &text) {
  m_dropdown->setCurrentText(text);
}