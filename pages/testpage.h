#ifndef TESTPAGE_H
#define TESTPAGE_H

#include <QWidget>

class TestPage : public QWidget {
  Q_OBJECT
public:
  explicit TestPage(QWidget *parent = nullptr);
};

#endif // TESTPAGE_H
