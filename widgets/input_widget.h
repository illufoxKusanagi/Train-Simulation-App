#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "../resources/input_type.h"
#include <QLabel>

class InputWidget : public QWidget {
  Q_OBJECT
public:
  explicit InputWidget(const InputType &inputType, QWidget *parent = nullptr);

private:
  QLabel *label;
};

#endif // INPUTWIDGET_H
