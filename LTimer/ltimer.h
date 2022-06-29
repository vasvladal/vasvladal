#ifndef LTIMER_H
#define LTIMER_H

#include <QElapsedTimer>
#include <QSharedPointer>
#include <QTimer>

class LTimer : public QObject
{
    Q_OBJECT

public:
    enum State {
        Inactive,
        Running,
        Paused
    };

    enum Type {
        Precise = Qt::TimerType::PreciseTimer,
        Coarse = Qt::TimerType::CoarseTimer,
        VeryCoarse = Qt::TimerType::VeryCoarseTimer,
        CoarseStabilized
    };

    enum StopPolicy {
        ByTimeout,
        ByRanOutOfTicks
    };

public:
    LTimer(QObject *parent = nullptr);
    ~LTimer();

    State state() const                         { return m_state; }

    void setDuraton(const int duration);
    int duration() const                        { return m_duration; }

    void setTicksInterval(const int interval);
    int ticksInterval() const                   { return m_ticksInterval; }

    void setTicksCount(const int count);
    int ticksCount() const                      { return m_ticksCount; }

    void setStopPolicy(const StopPolicy policy);
    StopPolicy stopPolicy() const               { return m_stopPolicy; }

    void setTimerType(const Type type);
    Type timerType() const                      { return m_timerType; }

    QElapsedTimer::ClockType clockType() const  { return m_elapsedTimer->clockType(); }
    bool isMonotonic() const                    { return m_elapsedTimer->isMonotonic(); }

    void start();
    void pause();
    void resume();
    void stop();    

    int elapsed() const;
    int remaining() const;

    int lastTickElapsed() const                 { return m_lastTickElapsed; }
    int lastTickRemaining() const;
    int lastTick() const                        { return m_lastTick; }

signals:
    void stateChanged(int);
    void tick(int);
    void stopped();
    void timeout();
    void ranOutOfTicks();

private:
    QElapsedTimer *m_elapsedTimer;
    QSharedPointer<QTimer> m_tickTimer;
    QSharedPointer<QTimer> m_durationTimer;

    State m_state;
    Type m_timerType;
    StopPolicy m_stopPolicy;

    int m_duration;
    int m_ticksInterval;
    int m_ticksCount;

    int m_elapsed;
    int m_lastTickElapsed;
    int m_lastTick;

private:
    void startTickTimer();
    void startDurationTimer();
    template<typename Functor> QSharedPointer<QTimer> newTimer(Functor functor) const;
    void _tick();
    int _tickInterval() const;
    void _stop();
    int _duration() const;
};

#endif // LTIMER_H
