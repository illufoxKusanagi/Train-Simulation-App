#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QLabel>

class InputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidget(const QString &label = "", const QString &unitLabel = "", QWidget *parent = nullptr);

private:
    QLabel *label;
};

#endif // INPUTWIDGET_H
