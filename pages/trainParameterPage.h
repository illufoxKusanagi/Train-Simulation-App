#ifndef TRAINPARAMETERPAGE_H
#define TRAINPARAMETERPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStringList>

class TrainParameterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainParameterWidget(QWidget *parent = nullptr);

private:
    QGridLayout *formLayout;
    QStringList labels;
    QStringList unitLabels;
};

#endif // TRAINPARAMETERPAGE_H
