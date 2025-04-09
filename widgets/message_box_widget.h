#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include "../styles/colors.h"
#include "../styles/text_style.h"
#include <QMessageBox>

class MessageBoxWidget : public QMessageBox {
  Q_OBJECT

public:
  explicit MessageBoxWidget();
};

#endif // MESSAGE_BOX_H
