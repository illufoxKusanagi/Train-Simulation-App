#ifndef ELECTRICAL_PARAMETER_PAGE_H
#define ELECTRICAL_PARAMETER_PAGE_H

#include "../widgets/input_widget.h"
#include <QGridLayout>
#include <QWidget>

class ElectricalParameterPage : public QWidget {
  Q_OBJECT
public:
  explicit ElectricalParameterPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *formContainer;
  QGridLayout *formLayout;

  void createInputs();
};

#endif // ELECTRICAL_PARAMETER_PAGE_H
