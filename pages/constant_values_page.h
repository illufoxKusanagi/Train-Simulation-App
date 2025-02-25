#ifndef CONSTANT_VALUES_PAGE_H
#define CONSTANT_VALUES_PAGE_H

#include "../models/constant_values.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "../widgets/input_widget.h"
#include <QGridLayout>
#include <QGroupBox>


class ConstantValuesPage : public QWidget {
  Q_OBJECT

public:
  explicit ConstantValuesPage(QWidget *parent = nullptr);
};

#endif // CONSTANT_VALUES_PAGE_H
