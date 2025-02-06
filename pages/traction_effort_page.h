#ifndef TRACTION_EFFORT_PAGE_H
#define TRACTION_EFFORT_PAGE_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class TractionEffortPage : public QWidget {
  Q_OBJECT
public:
  explicit TractionEffortPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
};

#endif // TRACTION_EFFORT_PAGE_H
