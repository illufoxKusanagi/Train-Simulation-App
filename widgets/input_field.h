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
  void setText(const QString &text);

private:
  QLineEdit *input;
  QLabel *unitLabel;
  void setSize(int width, int height);
};

#endif
