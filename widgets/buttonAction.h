#ifndef BUTTONACTION_H
#define BUTTONACTION_H

#include <QPushButton>

class ButtonAction : public QPushButton {
  Q_OBJECT

public:
  explicit ButtonAction(const QString &buttonText = "",
                        const QString &isRunButton = "",
                        QWidget *parent = nullptr);
  using QPushButton::setText;
  QString getText() const;

private:
  QPushButton *buttonAction;
};

#endif // BUTTONACTION_H
