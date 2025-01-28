#ifndef TRAINPARAMETERPAGE_H
#define TRAINPARAMETERPAGE_H

#include "../widgets/input_widget.h"
#include <QGridLayout>
#include <QWidget>

class TrainParameterPage : public QWidget {
  Q_OBJECT

public:
  explicit TrainParameterPage(QWidget *parent = nullptr);

private:
  QGridLayout *formLayout;
  QList<InputWidget *> inputWidgets;

  void setupInputs();
};

#endif // TRAINPARAMETERPAGE_H
