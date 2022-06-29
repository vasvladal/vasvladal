#include "ltimer.h"



LTimer::LTimer(QObject *parent) :
    QObject(parent),
    m_elapsedTimer(new QElapsedTimer),
    m_state(State::Inactive),
    m_timerType(Type::CoarseStabilized),
    m_stopPolicy(StopPolicy::ByTimeout),
    m_duration(-1),
    m_ticksInterval(1000),
    m_ticksCount(-1),
    m_elapsed(0),
    m_lastTickElapsed(0),
    m_lastTick(0)
{
}



LTimer::~LTimer()
{
    delete m_elapsedTimer;
}



void LTimer::setDuraton(const int duration)
{
    if (m_state == State::Inactive)
        m_duration = duration;
}



void LTimer::setTicksInterval(const int interval)
{
    if (m_state == State::Inactive)
        m_ticksInterval = interval;
}



void LTimer::setTicksCount(const int count)
{
    if (m_state == State::Inactive)
        m_ticksCount= count;
}



void LTimer::setTimerType(const Type type)
{
    if (m_state == State::Inactive)
        m_timerType = type;
}



void LTimer::setStopPolicy(const StopPolicy policy)
{
    if (m_state == State::Inactive)
        m_stopPolicy = policy;
}



void LTimer::start()
{
    m_elapsed = 0;
    m_lastTickElapsed = 0;
    m_lastTick = 0;

    m_elapsedTimer->start();

    startTickTimer();
    startDurationTimer();

    m_state = State::Running;
    emit stateChanged(m_state);
}



void LTimer::startTickTimer()
{
    if (m_ticksInterval >= 0 && m_ticksCount != 0) {
        if (m_tickTimer.isNull())
            m_tickTimer = newTimer(&LTimer::_tick);
        m_tickTimer->start(m_ticksInterval);
    }
    else
        m_tickTimer = QSharedPointer<QTimer>(nullptr);
}



void LTimer::startDurationTimer()
{
    if (m_stopPolicy == StopPolicy::ByTimeout && m_duration >= 0) {
        if (m_durationTimer.isNull())
            m_durationTimer = newTimer(&LTimer::_stop);
        m_durationTimer->start(m_duration);
    }
    else
        m_durationTimer = QSharedPointer<QTimer>(nullptr);
}



template<typename Functor>
QSharedPointer<QTimer> LTimer::newTimer(Functor functor) const
{
    QSharedPointer<QTimer> timer = QSharedPointer<QTimer>::create();
    timer->setSingleShot(true);

    if (m_timerType == Type::CoarseStabilized)
        timer->setTimerType(Qt::TimerType::CoarseTimer);
    else
        timer->setTimerType(static_cast<Qt::TimerType>(m_timerType));

    connect(timer.get(), &QTimer::timeout, this, functor);

    return timer;
}



void LTimer::pause()
{
    if (m_state != State::Running)
        return;

    m_elapsed += m_elapsedTimer->elapsed();

    if (!m_tickTimer.isNull())
        m_tickTimer->stop();

    if (!m_durationTimer.isNull())
        m_durationTimer->stop();

    m_state = State::Paused;
    emit stateChanged(m_state);
}



void LTimer::resume()
{
    if (m_state != State::Paused)
        return;

    m_elapsedTimer->restart();

    if (!m_tickTimer.isNull()) {
        int interval = (m_lastTick + 1) * m_ticksInterval - m_elapsed;
        if (interval < 0)
            interval = 0;
        m_tickTimer->start(interval);
    }

    if (!m_durationTimer.isNull())
        m_durationTimer->start(m_duration - m_elapsed);

    m_state = State::Running;
    emit stateChanged(m_state);
}



void LTimer::_tick()
{
    m_lastTickElapsed = elapsed();
    m_lastTick++;
    emit tick(m_lastTick);

    if (m_stopPolicy == StopPolicy::ByRanOutOfTicks && m_ticksCount > 0 && m_lastTick >= m_ticksCount)
        _stop();
    else
        m_tickTimer->start(_tickInterval());
}



int LTimer::_tickInterval() const
{
    if (m_timerType != Type::CoarseStabilized)
        return m_ticksInterval;

    const double interval = static_cast<double>(m_lastTick * m_ticksInterval * m_ticksInterval)
            / static_cast<double>(m_lastTickElapsed);

    return qRound(interval);
}



void LTimer::_stop()
{
    stop();

    emit stopped();

    if (m_stopPolicy == StopPolicy::ByTimeout)
        emit timeout();
    else
        emit ranOutOfTicks();
}



void LTimer::stop()
{
    if (m_state == State::Inactive)
        return;

    m_elapsed += m_elapsedTimer->elapsed();

    m_tickTimer = QSharedPointer<QTimer>(nullptr);
    m_durationTimer = QSharedPointer<QTimer>(nullptr);

    m_state = State::Inactive;
    emit stateChanged(m_state);
}



int LTimer::elapsed() const
{
    return m_state == State::Running
            ? m_elapsed + static_cast<int>(m_elapsedTimer->elapsed())
            : m_elapsed;
}



int LTimer::remaining() const
{
    const int d = _duration();

    if (d < 0)
        return -1;

    int _remaining = d - m_elapsed;

    if (m_state == State::Running)
        _remaining -= m_elapsedTimer->elapsed();

    return _remaining > 0 ? _remaining : 0;
}



int LTimer::lastTickRemaining() const
{
    const int d = _duration();

    if (d < 0)
        return -1;

    const int _remaining = d - m_lastTickElapsed;

    return _remaining > 0 ? _remaining : 0;
}



int LTimer::_duration() const
{
    return m_stopPolicy == StopPolicy::ByRanOutOfTicks
            ? m_ticksInterval * m_ticksCount
            : m_duration;
}
