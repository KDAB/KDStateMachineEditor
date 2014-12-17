/*
  quicksceneitem.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  This file may be distributed and/or modified under the terms of the
  GNU Lesser General Public License version 2.1 as published by the
  Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.

  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  Contact info@kdab.com if any conditions of this licensing are not
  clear to you.
*/

#ifndef KDSME_QUICK_QUICKSCENEITEM_H
#define KDSME_QUICK_QUICKSCENEITEM_H

#include <QQuickItem>

namespace KDSME {
class Element;
class StateMachineScene;
class Transition;
}

class QuickSceneItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(KDSME::StateMachineScene* scene READ scene WRITE setScene NOTIFY sceneChanged FINAL)
    Q_PROPERTY(KDSME::Element* element READ element WRITE setElement NOTIFY elementChanged FINAL)
    Q_PROPERTY(bool activeness READ activeness WRITE setActiveness NOTIFY activenessChanged FINAL)

public:
    explicit QuickSceneItem(QQuickItem* parent = nullptr);
    virtual ~QuickSceneItem();

    KDSME::StateMachineScene* scene() const;

    KDSME::Element* element() const;

    qreal activeness() const;
    void setActiveness(qreal activeness);

Q_SIGNALS:
    void sceneChanged(KDSME::StateMachineScene* scene);
    void elementChanged(KDSME::Element* element);
    void activenessChanged(qreal activeness);
    void clicked();

protected:
    virtual void setScene(KDSME::StateMachineScene* scene);
    virtual void setElement(KDSME::Element* element);

    QQuickItem* itemForElement(KDSME::Element* element) const;

    Q_INVOKABLE void sendClickEvent();

private:
    KDSME::StateMachineScene* m_scene;
    KDSME::Element* m_element;
    qreal m_activeness;
};

class QuickStateItem : public QuickSceneItem
{
    Q_OBJECT

public:
    explicit QuickStateItem(QQuickItem* parent = nullptr);
};

class QuickTransitionItem : public QuickSceneItem
{
    Q_OBJECT

public:
    explicit QuickTransitionItem(QQuickItem* parent = nullptr);

    virtual void setElement(KDSME::Element* element) override;

private Q_SLOTS:
    void updatePosition();

private:
    KDSME::Transition* toTransition() const;
};

#endif // KDSME_QUICK_QUICKSCENEITEM_H
