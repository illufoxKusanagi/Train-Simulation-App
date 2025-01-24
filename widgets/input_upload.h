#ifndef INPUTUPLOAD_H
#define INPUTUPLOAD_H

#include <QLabel>
#include <QPushButton>

class InputUpload : public QWidget {
  Q_OBJECT

public:
  explicit InputUpload(QWidget *parent = nullptr);

private:
  QPushButton *uploadButton;
  QLabel *uploadLabel;
};

#endif // INPUTUPLOAD_H
