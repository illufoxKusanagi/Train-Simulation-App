#include "inputInvalid.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include <qboxlayout.h>

InputInvalid::InputInvalid(const QString &label, QWidget *parent)
    : QWidget(parent),
      invalidLabel(new QLabel(" \'" + label +
                                  "\' input type isn't allowed. Use "
                                  "\'dropdown\', \'field\', or \'upload\'",
                              this)) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  invalidLabel->setStyleSheet(TextStyle::BodySmallBold() +
                              "color: " + Colors::Danger600.name() + ";");
  layout->addWidget(invalidLabel);
}
