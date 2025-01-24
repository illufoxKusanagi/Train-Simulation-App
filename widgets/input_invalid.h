#ifndef INPUTINVALID_H
#define INPUTINVALID_H

#include <QLabel>

class InputInvalid : public QWidget {
  Q_OBJECT
public:
  explicit InputInvalid(const QString &label = "", QWidget *parent = nullptr);

private:
  QLabel *invalidLabel;
};

#endif // INPUTINVALID_H
