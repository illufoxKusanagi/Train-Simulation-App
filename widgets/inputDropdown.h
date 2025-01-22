#ifndef INPUTDROPDOWN_H
#define INPUTDROPDOWN_H

#include <QComboBox>

class InputDropdown : public QWidget {
  Q_OBJECT
public:
  explicit InputDropdown(QWidget *parent = nullptr);

  QString currentText() const;
  void setCurrentText(const QString &text);

private:
  QComboBox *dropdown;
};

#endif // INPUTDROPDOWN_H
