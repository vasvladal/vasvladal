#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSpinBox>

#include "InitPlog.h"

static const QStringList s_stateNames = { "Inactive", "Running", "Paused" };
static const QStringList s_typeNames = { "Precise", "Coarse", "VeryCoarse", "CoarseStabilized" };
static const QStringList s_stopPolicyNames = { "By timeout", "By ran out of ticks" };

static constexpr int s_defaultDuration = 10000;
static constexpr int s_defaultTickInterval = 1000;
static constexpr int s_defaultTicksCount = 10;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_lTimer(new LTimer(this))
{
    ui->setupUi(this);
    setWindowTitle(qApp->applicationName());

    plog::init(plog::debug, &fileAppender)/*.addAppender(&consoleAppender)*/;
    //plog::init(plog::info, "ImitVip.log");

    ui->spinBox_duration->setValue(s_defaultDuration);
    ui->spinBox_ticksInterval->setValue(s_defaultTickInterval);
    ui->spinBox_ticksCount->setValue(s_defaultTicksCount);

    ui->comboBox_type->insertItems(0, s_typeNames);
    ui->comboBox_type->setCurrentIndex(m_lTimer->timerType());

    ui->comboBox_stopPolicy->insertItems(0, s_stopPolicyNames);
    ui->comboBox_stopPolicy->setCurrentIndex(m_lTimer->stopPolicy());

    m_lTimer->setDuraton(s_defaultDuration);
    m_lTimer->setTicksInterval(s_defaultTickInterval);
    m_lTimer->setTicksCount(s_defaultTicksCount);

    setProgressBarValue();

    connect(ui->pushButton_get, &QPushButton::released, this, &MainWindow::onGet);
    connect(ui->pushButton_start, &QPushButton::released, this, &MainWindow::start);
    connect(ui->pushButton_pause, &QPushButton::released, this, &MainWindow::pause);
    connect(ui->pushButton_resume, &QPushButton::released, m_lTimer, &LTimer::resume);
    connect(ui->pushButton_stop, &QPushButton::released, this, &MainWindow::stop);

    connect(m_lTimer, &LTimer::stateChanged, this, &MainWindow::onTimerStateChanged);
    connect(m_lTimer, &LTimer::tick, this, &MainWindow::onTimerTick);
    connect(m_lTimer, &LTimer::timeout, this, &MainWindow::onTimerTimeout);
    connect(m_lTimer, &LTimer::ranOutOfTicks, this, &MainWindow::onTimerRanOutOfTicks);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start()
{
    setWidgetsEnabled(false);

    m_lTimer->setDuraton(ui->spinBox_duration->value());
    m_lTimer->setTicksInterval(ui->spinBox_ticksInterval->value());
    m_lTimer->setTicksCount(ui->spinBox_ticksCount->value());
    m_lTimer->setTimerType(static_cast<LTimer::Type>(ui->comboBox_type->currentIndex()));
    m_lTimer->setStopPolicy(static_cast<LTimer::StopPolicy>(ui->comboBox_stopPolicy->currentIndex()));
    m_lTimer->start();

    setProgressBarValue();

    qDebug() << "started";
}



void MainWindow::pause()
{
    if (m_lTimer->state() != LTimer::Running)
        return;

    m_lTimer->pause();

    writeDebug(QStringLiteral("paused"));
}



void MainWindow::stop()
{
    setWidgetsEnabled(true);
    m_lTimer->stop();
    setProgressBarValue();
}



void MainWindow::onGet()
{
    ui->lineEdit_elapsed->setText(QString::number(m_lTimer->elapsed()));
    ui->lineEdit_remaining->setText(QString::number(m_lTimer->remaining()));
    ui->lineEdit_lastTickElapsed->setText(QString::number(m_lTimer->lastTickElapsed()));
    ui->lineEdit_lastTickRemaining->setText(QString::number(m_lTimer->lastTickRemaining()));
}



void MainWindow::onTimerStateChanged(const int state)
{

    QString qs = s_stateNames.at(state);

    ui->statusBar->showMessage(QLatin1String("Current state: ") + qs/*.arg(s_stateNames.at(state))*/);

    PLOGD <<" state = " <<state <<" qs = " <<qs;

}



void MainWindow::onTimerTick(const int tick)
{
    setProgressBarValue();
    writeDebug(QString("tick = %1").arg(tick));
}



void MainWindow::onTimerTimeout()
{
    setWidgetsEnabled(true);
    writeDebug(QStringLiteral("timeout"));
}



void MainWindow::onTimerRanOutOfTicks()
{
    setWidgetsEnabled(true);
    qDebug() << "ran out of ticks";
}



void MainWindow::writeDebug(const QString &text)
{
    qDebug().noquote()
            << text
            << " elapsed =" << m_lTimer->elapsed()
            << " remaining =" << m_lTimer->remaining()
            << " lastTickElapsed =" << m_lTimer->lastTickElapsed()
            << " lastTickRemaining =" << m_lTimer->lastTickRemaining();
}



void MainWindow::setWidgetsEnabled(const bool enabled)
{
    ui->spinBox_duration->setEnabled(enabled);
    ui->spinBox_ticksInterval->setEnabled(enabled);
    ui->spinBox_ticksCount->setEnabled(enabled);
    ui->comboBox_type->setEnabled(enabled);
    ui->comboBox_stopPolicy->setEnabled(enabled);
}



void MainWindow::setProgressBarValue()
{
    ui->progressBar->setValue(m_lTimer->lastTick());

    if (m_lTimer->ticksCount() > 0) {
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(m_lTimer->ticksCount());
    }
    else {
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(INT_MAX);
    }

    const QString text = QString("%1 / %2").arg(m_lTimer->lastTick()).arg(m_lTimer->ticksCount());
    ui->progressBar->setFormat(text);
}

