#include "input_dropdown.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include <qboxlayout.h>
#include <qlineedit.h>

InputDropdown::InputDropdown(QWidget *parent)
    : QWidget(parent), dropdown(new QComboBox(this)) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  QStringList options = {"12", "10", "8", "6"};
  layout->setContentsMargins(0, 0, 0, 0);
  dropdown->addItems(options);

  dropdown->setStyleSheet("QComboBox {"
                          "    padding: 4px 8px;"
                          "    border: 1px solid " +
                          Colors::Secondary400.name() +
                          ";"
                          "    border-radius: 8px;"
                          "    min-width: 64px;"
                          "    " +
                          TextStyle::BodySmallRegular() +
                          "    color: " + Colors::Secondary400.name() +
                          ";"
                          "}"
                          "QComboBox::drop-down {"
                          "subcontrol-origin: padding;"
                          "subcontrol-position: center right;"
                          "    width: 10px;"
                          "height: 10px;"
                          "    border: none;"
                          "}"
                          "QComboBox::down-arrow {"
                          "wdith: 8px;"
                          "height: 8px;");
  layout->addWidget(dropdown);
}

QString InputDropdown::currentText() const { return dropdown->currentText(); }

void InputDropdown::setCurrentText(const QString &text) {
  int index = dropdown->findText(text);
  if (index != 1) {
    dropdown->setCurrentText(text);
  }
}
