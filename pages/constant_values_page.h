#ifndef CONSTANT_VALUES_PAGE_H
#define CONSTANT_VALUES_PAGE_H

#include "../widgets/input_widget.h"
#include <QGridLayout>

class ConstantValuesPage : public QWidget {
  Q_OBJECT

public:
  explicit ConstantValuesPage(QWidget *parent = nullptr);

private:
  QGridLayout *formLayout;
};

#endif // CONSTANT_VALUES_PAGE_H
