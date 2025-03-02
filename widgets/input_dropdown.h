#ifndef INPUTDROPDOWN_H
#define INPUTDROPDOWN_H

#include "../styles/colors.h"
#include "../styles/text_style.h"
#include <QAbstractItemView>
#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QStringList>

class InputDropdown : public QWidget {
  Q_OBJECT
public:
  explicit InputDropdown(QWidget *parent = nullptr);

  QString currentText() const;
  void setCurrentText(const QString &text);
  void setSize(int width, int height);

private:
  QComboBox *m_dropdown;
};

#endif // INPUTDROPDOWN_H
