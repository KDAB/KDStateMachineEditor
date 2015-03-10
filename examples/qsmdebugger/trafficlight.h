#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <QWidget>

class QStateMachine;

/**
 * Copy of the infamous traffic light example from Qt
 *
 * See http://doc-snapshot.qt-project.org/qt5-5.4/qtwidgets-statemachine-trafficlight-example.html
 */
class TrafficLight : public QWidget
{
    Q_OBJECT

public:
    TrafficLight(QWidget *parent = 0);

    QStateMachine *machine() const;

private:
    QStateMachine* m_machine;
};

#endif
