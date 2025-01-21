#include "inputField.h"
#include <qboxlayout.h>

InputField::InputField(const QString &unitText, QWidget *parent)
    : QWidget(parent), input(new QLineEdit(this)), unitLabel(new QLabel(unitText, this)){
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    layout->addWidget(input);
    layout->addWidget(unitLabel);

    input->setStyleSheet("min-width: 100px; max-width: 144px");
    unitLabel->setStyleSheet("color: gray;");
}


QString InputField::text() const {
    return input->text();
}

void InputField::setText(const QString &text) {
    input->setText(text);
}
