/*
  statemachinescene.h

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


#ifndef KDSME_VIEW_STATEMACHINESCENE_H
#define KDSME_VIEW_STATEMACHINESCENE_H

#include "kdsme_view_export.h"
#include "abstractview.h"

#include <QObject>

namespace KDSME {
class Element;
class Layouter;
class LayoutProperties;
class State;
class StateMachine;
class StateModel;
class Transition;

class KDSME_VIEW_EXPORT StateMachineScene : public AbstractView
{
    Q_OBJECT
    Q_PROPERTY(KDSME::StateMachine* stateMachine READ stateMachine WRITE setStateMachine NOTIFY stateMachineChanged)
    Q_PROPERTY(KDSME::Element* currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
    Q_PROPERTY(KDSME::LayoutProperties* layoutProperties READ layoutProperties CONSTANT)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged FINAL)

public:
    explicit StateMachineScene(QQuickItem* parent = nullptr);
    virtual ~StateMachineScene();

    StateModel* stateModel() const;
    virtual void setModel(QAbstractItemModel* model) override;

    StateMachine* stateMachine() const;
    /// Ownership is not transferred
    void setStateMachine(StateMachine* stateMachine);

    Layouter* layouter() const;
    /// Ownership is transferred
    void setLayouter(Layouter* layouter);

    qreal zoom() const;
    void setZoom(qreal zoom);

    void zoomBy(qreal zoom);

    LayoutProperties* layoutProperties() const;

    Q_INVOKABLE void collapseItem(KDSME::State* state);
    Q_INVOKABLE void expandItem(KDSME::State* state);
    bool isItemExpanded(KDSME::State* state) const;
    void setItemExpanded(KDSME::State* state, bool expand);

    Q_INVOKABLE bool isItemSelected(KDSME::Element* item);
    Q_INVOKABLE void setItemSelected(KDSME::Element* item, bool selected);

    KDSME::Element* currentItem() const;
    Q_INVOKABLE void setCurrentItem(KDSME::Element* item);

    Q_INVOKABLE KDSME::Element* currentState();

public Q_SLOTS:
    void layout();

Q_SIGNALS:
    void stateMachineChanged(KDSME::StateMachine* stateMachine);
    void currentItemChanged(KDSME::Element* currentItem);
    void zoomChanged(qreal zoom);

protected Q_SLOTS:
    virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

    virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) override;
    virtual void rowsInserted(const QModelIndex& parent, int start, int end) override;
    virtual void layoutChanged() override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::StateMachineScene*)

#endif // VIEW_H
