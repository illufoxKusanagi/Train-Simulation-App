#include "traction_effort_page.h"

TractionEffortPage::TractionEffortPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  setLayout(mainLayout);
  QLabel *label = new QLabel("Traction Effort Page", this);
}
