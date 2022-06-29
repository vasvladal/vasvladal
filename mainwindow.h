#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "LTimer/ltimer.h"
#include <QMainWindow>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LTimer *m_lTimer;
private:
    void start();
    void pause();
    void stop();
    void onGet();

    void onTimerStateChanged(const int state);
    void onTimerTick(const int tick);
    void onTimerTimeout();
    void onTimerRanOutOfTicks();

    void writeDebug(const QString &text);

    void setWidgetsEnabled(const bool enabled);
    void setProgressBarValue();
};
#endif // MAINWINDOW_H
