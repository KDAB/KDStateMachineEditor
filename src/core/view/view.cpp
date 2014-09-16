/*
  view.cpp

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

#include "view.h"

#include "element.h"
#include "layerwiselayouter.h"
#include "layouter.h"
#include "layoutitem.h"
#include "layoutitemmodel.h"
#include "layoutproperties.h"
#include "layoututils.h"
#include "elementmodel.h"

#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QItemSelectionModel>
#include <QQmlEngine>
#include <QTimer>

using namespace KDSME;

namespace {

QList<Element*> elementsAt(const QModelIndex& parent, int start, int end)
{
    const QAbstractItemModel* model = parent.model();
    Q_ASSERT(model);

    QList<Element*> elements;
    for (int i = start; i <= end; ++i) {
        Element* element = model->index(i, 0, parent).data(StateModel::ElementRole).value<Element*>();
        Q_ASSERT(element);
        elements << element;
    }
    return elements;
}

}

struct View::Private
{
    Private(View* view);

    StateLayoutItem* importState(State* state);

    void updateChildItemVisibility(StateLayoutItem* state, bool expand);
    void doDelayedImport();

    void setRootLayoutItem(StateLayoutItem* root);

    void importTransitions(State* state);
    TransitionLayoutItem* importTransition(Transition* transition);

    View* q;

    QPointer<StateMachine> m_stateMachine;
    Layouter* m_layouter;
    LayoutProperties* m_properties;

    // TODO: Use Boost Bimap?
    StateLayoutItem* m_root;
    QMap<Element*, LayoutItem*> m_elementToLayoutItemMap;

    StateLayoutItem* m_expandedItem;
    QTimer* m_delayedImportTimer;
    bool m_reimportRequired;
};

View::Private::Private(View* view)
    : q(view)
    , m_layouter(new LayerwiseLayouter(q))
    , m_properties(new LayoutProperties(q))
    , m_root(0)
    , m_expandedItem(nullptr)
    , m_delayedImportTimer(new QTimer(q))
    , m_reimportRequired(false)
{
}

View::View(QObject* parent)
    : AbstractView(parent)
    , d(new Private(this))

{
    d->m_delayedImportTimer->setSingleShot(true);
    connect(d->m_delayedImportTimer, SIGNAL(timeout()), this, SLOT(import()));
}

View::~View()
{
}

LayoutProperties* View::layoutProperties() const
{
    return d->m_properties;
}

void View::collapseItem(StateLayoutItem* state)
{
    if (!state || !state->isExpanded())
        return;

    state->setExpanded(false);
    d->updateChildItemVisibility(state, false);
    emit contentsUpdated(state);
    d->doDelayedImport();
}

void View::expandItem(StateLayoutItem* state)
{
    if (!state || state->isExpanded())
        return;

    state->setExpanded(true);

    // we delay showing the child items until the layout has been done
    d->m_expandedItem = state;

    d->doDelayedImport();
}

bool View::isItemExpanded(StateLayoutItem* state) const
{
    return state ? state->isExpanded() : false;
}

void View::setItemExpanded(StateLayoutItem* state, bool expand)
{
    if (expand) {
        expandItem(state);
    } else {
        collapseItem(state);
    }
}

bool View::isItemSelected(LayoutItem* item)
{
    if (!stateModel() || !item || !item->element())
        return false;

    const QModelIndex index = stateModel()->indexForObject(item->element());
    return selectionModel()->isSelected(index);
}

void View::setItemSelected(LayoutItem* item, bool selected)
{
    if (!stateModel() || !item || !item->element())
        return;

    const QModelIndex index = stateModel()->indexForObject(item->element());
    selectionModel()->select(index, (selected ? QItemSelectionModel::Select : QItemSelectionModel::Deselect));
}

void View::setCurrentItem(LayoutItem* item)
{
    if (!stateModel() || !item || !item->element())
        return;

    const QModelIndex index = stateModel()->indexForObject(item->element());
    setCurrentIndex(index);
}

Element* View::currentState()
{
    Element *element = selectionModel()->currentIndex().data(StateModel::ElementRole).value<Element*>();
    if (!element || element->type() == Element::ElementType)
        return nullptr;

    if (element->type() == Element::TransitionType)
        element = static_cast<Transition*>(element)->sourceState();

    QQmlEngine::setObjectOwnership(element, QQmlEngine::CppOwnership);
    return element;
}

StateMachine* View::stateMachine() const
{
    return d->m_stateMachine;
}

void View::setStateMachine(StateMachine* stateMachine)
{
    if (d->m_stateMachine == stateMachine)
        return;

    emit contentsAboutToBeChanged();

    d->setRootLayoutItem(nullptr);
    d->m_stateMachine = stateMachine;
    import();

    emit stateMachineChanged(d->m_stateMachine);
    emit contentsChanged();
}

Layouter* View::layouter() const
{
    return d->m_layouter;
}

void View::setLayouter(Layouter* layouter)
{
    if (d->m_layouter == layouter)
        return;

    if (d->m_layouter) {
        delete d->m_layouter;
    }

    d->m_layouter = layouter;

    if (d->m_layouter) {
        d->m_layouter->setParent(this);
    }
    layout();
    emit contentsUpdated(rootLayoutItem());
}

void View::import()
{
    if (!d->m_stateMachine) {
        return;
    }

    setState(ItemLayoutState);
    if (!d->m_root || d->m_reimportRequired) {
        // HACK: ParentChange state is active, and it will behave very odd if the
        // parent it is controlling is modified.
        // TODO: Check how to disable/avoid active ParentChange during the actual modification of the layout
        if (selectionModel())
            selectionModel()->clear();

        emit contentsAboutToBeChanged();

        // import
        StateLayoutItem* root = d->importState(d->m_stateMachine);
        if (root) {
            d->importTransitions(d->m_stateMachine);
            d->setRootLayoutItem(root);
        } else {
            qDebug() << "Importing failed";
        }
        d->m_reimportRequired = false;

        emit contentsChanged();
    }

    LayoutUtils::fixupLayout(d->m_root);

    if (d->m_expandedItem) {
        // show expanded item again
        d->updateChildItemVisibility(d->m_expandedItem, true);
        emit contentsUpdated(d->m_expandedItem);
        d->m_expandedItem = nullptr;
    }
    setState(NoState);
}

void View::layout()
{
    if (!d->m_layouter || !d->m_root) {
        return;
    }

    d->m_layouter->layout(d->m_root, this);
}

StateModel* View::stateModel() const
{
    return qobject_cast<StateModel*>(model());
}

void View::setModel(QAbstractItemModel* model)
{
    StateModel* stateModel = qobject_cast<StateModel*>(model);
    if (!stateModel) {
        qWarning() << "Invalid model class type, expected StateModel instance";
    }

    KDSME::AbstractView::setModel(stateModel);
}

StateLayoutItem* View::Private::importState(State* state)
{
    if (!state)
        return nullptr;

    auto item = qobject_cast<StateLayoutItem*>(m_elementToLayoutItemMap.value(state));
    if (!item) {
        item = new StateLayoutItem;
        m_elementToLayoutItemMap[state] = item;
    }
    State* parentState = state->parentState();
    StateLayoutItem* parentItem = (parentState ? qobject_cast<StateLayoutItem*>(m_elementToLayoutItemMap[parentState]) : nullptr);
    LayoutUtils::moveToParent(item, parentItem);
    item->setView(q);
    item->setElement(state);

    // recursive import
    Q_FOREACH (State* state, state->childStates()) {
        importState(state);
    }

    return item;
}

void View::Private::updateChildItemVisibility(StateLayoutItem* state, bool expand)
{
    if (!state)
        return;

    LayoutWalker walker(LayoutWalker::PreOrderTraversal);
    walker.walkChildren(state, [&](LayoutItem* i) -> LayoutWalker::VisitResult {
        if (TransitionLayoutItem* transition = qobject_cast<TransitionLayoutItem*>(i)) {
            // Avoid hiding transitions from states that are collapsed but still visible
            // which have a sibling state as their target
            if (transition->sourceState()->isVisible() && transition->sourceState()->isSiblingOf(transition->targetState())) {
                i->setVisible(true);
                return LayoutWalker::RecursiveWalk;
            }
        }

        i->setVisible(expand);
        return LayoutWalker::RecursiveWalk;
    });
}

void View::Private::doDelayedImport()
{
    m_delayedImportTimer->start();
}

void View::Private::importTransitions(State* state)
{
    if (!state)
        return;

    foreach (Transition* transition, state->transitions()) {
        importTransition(transition);
    }

    foreach (State* childState, state->childStates()) {
        importTransitions(childState);
    }
}

TransitionLayoutItem* View::Private::importTransition(Transition* transition)
{
    if (!transition) {
        return nullptr;
    }

    auto item = qobject_cast<TransitionLayoutItem*>(m_elementToLayoutItemMap.value(transition));
    if (!item) {
        item = new TransitionLayoutItem;
        m_elementToLayoutItemMap[transition] = item;
    }


    StateLayoutItem* parentState = qobject_cast<StateLayoutItem*>(m_elementToLayoutItemMap[transition->sourceState()]);
    StateLayoutItem* targetState = qobject_cast<StateLayoutItem*>(m_elementToLayoutItemMap.value(transition->targetState()));
    Q_ASSERT(parentState);
#if 0
    if (!targetState) {
        qDebug() << "Transition" << transition->label() << "has no target state set, ignoring";
        return nullptr;
    }
#endif

    LayoutUtils::moveToParent(item, parentState);
    item->setView(q);
    item->setTargetState(targetState);
    item->setElement(transition);
    return item;
}

StateLayoutItem* View::rootLayoutItem() const
{
    return d->m_root;
}

void View::Private::setRootLayoutItem(StateLayoutItem* root)
{
    if (m_root == root)
        return;

    if (m_root) {
        // Reset the view to the initial state (delete current layout structure)
        m_elementToLayoutItemMap.clear();
        delete m_root;
    }

    m_root = root;

    if (m_root) {
        m_root->setParent(q);
    }
    emit q->rootLayoutItemChanged(m_root);
}

QList<LayoutItem*> View::layoutItems() const
{
    return d->m_elementToLayoutItemMap.values();
}

LayoutItem* View::layoutItemForElement(Element* element)
{
    return d->m_elementToLayoutItemMap.value(element);
}

void View::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    AbstractView::currentChanged(current, previous);

    LayoutItem* currentItem = layoutItemForElement(current.data(StateModel::ElementRole).value<Element*>());
    LayoutItem* previousItem = layoutItemForElement(previous.data(StateModel::ElementRole).value<Element*>());
    if (!currentItem && !previousItem) {
        // something went wrong
        return;
    }

    if (currentItem) {
        currentItem->setSelected(true);
    }
    if (previousItem) {
        previousItem->setSelected(false);
    }

    emit currentItemChanged(current, previous);
}

void View::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    AbstractView::rowsAboutToBeRemoved(parent, start, end);

    auto elements = elementsAt(parent, start, end);
    if (elements.isEmpty()) {
        return;
    }

    emit contentsAboutToBeChanged();
    foreach (Element* element, elements) {
        LayoutItem* item = d->m_elementToLayoutItemMap.take(element);
        delete item;
    }
    emit contentsChanged();
}

void View::rowsInserted(const QModelIndex& parent, int start, int end)
{
    AbstractView::rowsInserted(parent, start, end);

    d->m_reimportRequired = true;
    d->doDelayedImport();
}

void View::layoutChanged()
{
    AbstractView::layoutChanged();

    d->m_reimportRequired = true;
    d->doDelayedImport();
}

#include "moc_view.cpp"
