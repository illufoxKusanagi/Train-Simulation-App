#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include "../styles/colors.h"
#include "../styles/text_style.h"
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>

class InputField : public QWidget {
  Q_OBJECT
public:
  explicit InputField(const QString &unitText = "", QWidget *parent = nullptr);
  QString text() const;
  double setValue(double value);
  void setPlaceholder();
  void setReadOnly(bool isReadOnly);

private:
  QLineEdit *m_input;
  QLabel *m_unitLabel;
};

#endif
