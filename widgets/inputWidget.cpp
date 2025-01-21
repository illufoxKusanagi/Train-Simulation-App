#include "inputWidget.h"
#include <qboxlayout.h>
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include "inputField.h"

InputWidget::InputWidget(const QString &labelText, const QString &unitLabel, QWidget *parent)
    : QWidget(parent), label(new QLabel(labelText, this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addWidget(label);
    label->setStyleSheet(TextStyle::BodyMediumRegular() + "color: " + Colors::Secondary700.name() + ";");
    InputField *inputField = new InputField(unitLabel, this);
    layout->addWidget(inputField);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    adjustSize();
}
