#include "chart_widget.h"

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)), chartLayout(nullptr),
      chartWidget(nullptr) {
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(16);
}
