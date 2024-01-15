/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_QUICK_QUICKSCENEITEM_P_H
#define KDSME_QUICK_QUICKSCENEITEM_P_H

#include <QQuickItem>

#include "statemachinescene.h"

#include <QPainterPath>

namespace KDSME {
class Element;
class Transition;
}

class QuickSceneItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(KDSME::StateMachineScene *scene READ scene WRITE setScene NOTIFY sceneChanged FINAL)
    Q_PROPERTY(KDSME::Element *element READ element WRITE setElement NOTIFY elementChanged FINAL)
    Q_PROPERTY(QPainterPath shape READ shape WRITE setShape NOTIFY shapeChanged FINAL)
    Q_PROPERTY(qreal activeness READ activeness WRITE setActiveness NOTIFY activenessChanged FINAL)

public:
    explicit QuickSceneItem(QQuickItem *parent = nullptr);
    ~QuickSceneItem();

    KDSME::StateMachineScene *scene() const;

    KDSME::Element *element() const;

    QPainterPath shape() const;
    void setShape(const QPainterPath &shape);

    qreal activeness() const;
    void setActiveness(qreal activeness);

Q_SIGNALS:
    void sceneChanged(KDSME::StateMachineScene *scene);
    void elementChanged(KDSME::Element *element);
    void shapeChanged(const QPainterPath &shape);
    void activenessChanged(qreal activeness);
    void clicked();

protected:
    virtual void setScene(KDSME::StateMachineScene *scene);
    virtual void setElement(KDSME::Element *element);

    bool contains(const QPointF &point) const override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QQuickItem *itemForElement(KDSME::Element *element) const;

    Q_INVOKABLE void sendClickEvent();

private:
    KDSME::StateMachineScene *m_scene;
    KDSME::Element *m_element;
    QPainterPath m_shape;
    qreal m_activeness;
};

class QuickStateItem : public QuickSceneItem
{
    Q_OBJECT

public:
    explicit QuickStateItem(QQuickItem *parent = nullptr);
};

class QuickTransitionItem : public QuickSceneItem
{
    Q_OBJECT

public:
    explicit QuickTransitionItem(QQuickItem *parent = nullptr);

    void setElement(KDSME::Element *element) override;

private Q_SLOTS:
    void updatePosition();
    void updateSource();
    void updateTarget();

private:
    KDSME::Transition *toTransition() const;

    QMetaObject::Connection m_sourceStateConnection;
    QMetaObject::Connection m_targetStateConnection;
};

#endif // KDSME_QUICK_QUICKSCENEITEM_P_H
