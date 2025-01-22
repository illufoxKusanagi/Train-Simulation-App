#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QPushButton>
#include <QWidget>

class ActionButton : public QPushButton {
  Q_OBJECT

public:
  explicit ActionButton(const QString &buttonText = "",
                        QWidget *parent = nullptr);
  using QPushButton::setText;
  QString getText() const;

private:
  QPushButton *actionButton;
};

#endif // ACTIONBUTTON_H
