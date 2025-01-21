#ifndef TRAINPARAMETERPAGE_H
#define TRAINPARAMETERPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStringList>

class TrainParameterWidget : public QWidget {
    Q_OBJECT

public:
    explicit TrainParameterWidget(QWidget *parent = nullptr);

private:
    QGridLayout *formLayout; // Layout untuk form parameter kereta
    QStringList labels;      // Label untuk parameter
    QStringList unitLabels;  // Satuan untuk parameter
};

#endif // TRAINPARAMETERPAGE_H
