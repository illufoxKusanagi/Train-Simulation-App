#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void navigateToPage(int pageIndex);

private:
    QWidget *menuWidget;
    QStackedWidget *stackedWidget;

    void leftPanel();
    void constantValuesPage();
    void trainParameterPage();
    void runningParameterPage();
    void trackParameterPage();
    void electricalParameterPage();
};

#endif
