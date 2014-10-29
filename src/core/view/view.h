/*
  view.h

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


#ifndef KDSME_VIEW_VIEW_H
#define KDSME_VIEW_VIEW_H

#include "kdsme_core_export.h"
#include "abstractview.h"

#include <QObject>

namespace KDSME {

class Element;
class Layouter;
class LayoutItem;
class LayoutProperties;
class State;
class StateLayoutItem;
class StateMachine;
class StateModel;
class Transition;
class TransitionLayoutItem;
class View;

class KDSME_CORE_EXPORT View : public AbstractView
{
    Q_OBJECT
    Q_PROPERTY(KDSME::StateMachine* stateMachine READ stateMachine WRITE setStateMachine NOTIFY stateMachineChanged)
    Q_PROPERTY(KDSME::LayoutItem* currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
    Q_PROPERTY(KDSME::LayoutItem* rootLayoutItem READ rootLayoutItem NOTIFY rootLayoutItemChanged)
    Q_PROPERTY(KDSME::LayoutProperties* layoutProperties READ layoutProperties CONSTANT)

public:
    explicit View(QObject* parent = 0);
    virtual ~View();

    StateModel* stateModel() const;
    virtual void setModel(QAbstractItemModel* model) Q_DECL_OVERRIDE;

    StateMachine* stateMachine() const;
    /// Ownership is not transferred
    void setStateMachine(StateMachine* stateMachine);

    Layouter* layouter() const;
    /// Ownership is transferred
    void setLayouter(Layouter* layouter);

    LayoutProperties* layoutProperties() const;

    Q_INVOKABLE void collapseItem(KDSME::StateLayoutItem* state);
    Q_INVOKABLE void expandItem(KDSME::StateLayoutItem* state);
    bool isItemExpanded(KDSME::StateLayoutItem* state) const;
    void setItemExpanded(KDSME::StateLayoutItem* state, bool expand);

    Q_INVOKABLE bool isItemSelected(KDSME::LayoutItem* item);
    Q_INVOKABLE void setItemSelected(KDSME::LayoutItem* item, bool selected);

    KDSME::LayoutItem* currentItem() const;
    Q_INVOKABLE void setCurrentItem(KDSME::LayoutItem* item);

    Q_INVOKABLE KDSME::Element* currentState();

    StateLayoutItem* rootLayoutItem() const;

    QList<LayoutItem*> layoutItems() const;
    LayoutItem* layoutItemForElement(Element* element) const;

public Q_SLOTS:
    void import();
    void layout();

Q_SIGNALS:
    void stateMachineChanged(KDSME::StateMachine* stateMachine);
    void rootLayoutItemChanged(KDSME::LayoutItem* root);

    void contentsAboutToBeChanged();
    void contentsChanged();

    /// Notify that properties of any descendant of @p item including @p item may have changed
    void contentsUpdated(LayoutItem* item);

    void currentItemChanged(KDSME::LayoutItem* currentItem);

protected Q_SLOTS:
    virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous) Q_DECL_OVERRIDE;

    virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) Q_DECL_OVERRIDE;
    virtual void rowsInserted(const QModelIndex& parent, int start, int end) Q_DECL_OVERRIDE;
    virtual void layoutChanged() Q_DECL_OVERRIDE;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::View*)

#endif // VIEW_H
