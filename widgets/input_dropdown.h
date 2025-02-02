#ifndef INPUTDROPDOWN_H
#define INPUTDROPDOWN_H

#include "../styles/colors.h"
#include "../styles/text_style.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QStringList>

class InputDropdown : public QWidget {
  Q_OBJECT
public:
  explicit InputDropdown(QWidget *parent = nullptr);

  QString currentText() const;
  void setCurrentText(const QString &text);

private:
  QComboBox *dropdown;
  void setSize(int width, int height);
};

#endif // INPUTDROPDOWN_H
