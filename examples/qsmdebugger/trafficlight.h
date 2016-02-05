#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <QWidget>

class QStateMachine;

/**
 * Copy of the infamous traffic light example from Qt
 *
 * See http://doc.qt.io/qt-5/qtwidgets-statemachine-trafficlight-example.html
 */
class TrafficLight : public QWidget
{
    Q_OBJECT

public:
    TrafficLight(QWidget *parent = nullptr);

    QStateMachine *machine() const;

private:
    QStateMachine* m_machine;
};

#endif
