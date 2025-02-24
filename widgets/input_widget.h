#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "../resources/input_type.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "input_dropdown.h"
#include "input_field.h"
#include "input_invalid.h"
#include "input_upload.h"
#include <QBoxLayout>
#include <QLabel>

struct InputSize {
  int width;
  int height;
};

class InputWidget : public QWidget {
  Q_OBJECT
public:
  explicit InputWidget(const InputType &inputType, QWidget *parent = nullptr);

private:
  QLabel *m_label;
  InputField *m_inputField;
  InputDropdown *m_inputDropdown;
  InputUpload *m_inputUpload;
  InputInvalid *m_inputInvalid;
  void setPlaceholder(const QString &placeholder);
  void setValue(double value);
  void getValue();
};

#endif // INPUTWIDGET_H
