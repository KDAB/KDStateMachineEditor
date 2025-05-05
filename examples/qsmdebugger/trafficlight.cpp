/**
 * Copy of the infamous traffic light example from Qt
 *
 * See https://doc.qt.io/qt-5/qtwidgets-statemachine-trafficlight-example.html
 */

#include "trafficlight.h"

#include <QFinalState>
#include <QPainter>
#include <QStateMachine>
#include <QTimer>
#include <QVBoxLayout>

//! [0]
class LightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    LightWidget(const QColor &color, QWidget *parent = nullptr)
        : QWidget(parent)
        , m_color(color)
        , m_on(false)
    {
    }

    bool isOn() const
    {
        return m_on;
    }
    void setOn(bool on)
    {
        if (on == m_on)
            return;
        m_on = on;
        update();
    }

public Q_SLOTS:
    void turnOff()
    {
        setOn(false);
    }
    void turnOn()
    {
        setOn(true);
    }

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE
    {
        if (!m_on)
            return;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(m_color);
        painter.drawEllipse(0, 0, width(), height());
    }

private:
    QColor m_color;
    bool m_on;
};
//! [0]

//! [1]
class TrafficLightWidget : public QWidget
{
    Q_OBJECT

public:
    TrafficLightWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *vbox = new QVBoxLayout(this);
        m_red = new LightWidget(Qt::red);
        vbox->addWidget(m_red);
        m_yellow = new LightWidget(Qt::yellow);
        vbox->addWidget(m_yellow);
        m_green = new LightWidget(Qt::green);
        vbox->addWidget(m_green);
        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::black);
        setPalette(pal);
        setAutoFillBackground(true);
    }

    LightWidget *redLight() const
    {
        return m_red;
    }
    LightWidget *yellowLight() const
    {
        return m_yellow;
    }
    LightWidget *greenLight() const
    {
        return m_green;
    }

private:
    LightWidget *m_red;
    LightWidget *m_yellow;
    LightWidget *m_green;
};

QState *createLightState(LightWidget *light, int duration, QState *parent = nullptr)
{
    QState *lightState = new QState(parent);
    QTimer *timer = new QTimer(lightState);
    timer->setInterval(duration);
    timer->setSingleShot(true);
    QState *timing = new QState(lightState);
    QObject::connect(timing, SIGNAL(entered()), light, SLOT(turnOn()));
    QObject::connect(timing, SIGNAL(entered()), timer, SLOT(start()));
    QObject::connect(timing, SIGNAL(exited()), light, SLOT(turnOff()));
    QFinalState *done = new QFinalState(lightState);
    timing->addTransition(timer, SIGNAL(timeout()), done);
    lightState->setInitialState(timing);
    return lightState;
}


TrafficLight::TrafficLight(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    TrafficLightWidget *widget = new TrafficLightWidget();
    vbox->addWidget(widget);
    vbox->setContentsMargins(0, 0, 0, 0);

    QStateMachine *machine = new QStateMachine(this);
    QState *redGoingYellow = createLightState(widget->redLight(), 3000);
    redGoingYellow->setObjectName(QStringLiteral("redGoingYellow"));
    QState *yellowGoingGreen = createLightState(widget->yellowLight(), 1000);
    yellowGoingGreen->setObjectName(QStringLiteral("yellowGoingGreen"));
    redGoingYellow->addTransition(redGoingYellow, SIGNAL(finished()), yellowGoingGreen);
    QState *greenGoingYellow = createLightState(widget->greenLight(), 3000);
    greenGoingYellow->setObjectName(QStringLiteral("greenGoingYellow"));
    yellowGoingGreen->addTransition(yellowGoingGreen, SIGNAL(finished()), greenGoingYellow);
    QState *yellowGoingRed = createLightState(widget->yellowLight(), 1000);
    yellowGoingRed->setObjectName(QStringLiteral("yellowGoingRed"));
    greenGoingYellow->addTransition(greenGoingYellow, SIGNAL(finished()), yellowGoingRed);
    yellowGoingRed->addTransition(yellowGoingRed, SIGNAL(finished()), redGoingYellow);

    machine->addState(redGoingYellow);
    machine->addState(yellowGoingGreen);
    machine->addState(greenGoingYellow);
    machine->addState(yellowGoingRed);
    machine->setInitialState(redGoingYellow);
    machine->start();
    m_machine = machine;
}

QStateMachine *TrafficLight::machine() const
{
    return m_machine;
}

#include "trafficlight.moc"
