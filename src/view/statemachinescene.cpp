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

#include "statemachinescene.h"
#include "statemachinescene_p.h"

#include "debug.h"
#include "state.h"
#include "transition.h"
#include "layerwiselayouter.h"
#include "layouter.h"
#include "layoutproperties.h"
#include "layoututils.h"
#include "elementmodel.h"
#include "elementwalker.h"

#include <QDir>
#include <QElapsedTimer>
#include <QItemSelectionModel>
#include <QMatrix>
#include <QPainterPath>
#include <QQmlEngine>

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

StateMachineScene::Private::Private(StateMachineScene* view)
    : q(view)
    , m_layouter(new LayerwiseLayouter(q))
    , m_properties(new LayoutProperties(q))
    , m_zoom(1.0)
    , m_expandedItem(nullptr)
{
}

StateMachineScene::StateMachineScene(QQuickItem* parent)
    : AbstractView(parent)
    , d(new Private(this))
{
    setModel(new StateModel(this));
}

StateMachineScene::~StateMachineScene()
{
}

LayoutProperties* StateMachineScene::layoutProperties() const
{
    return d->m_properties;
}

void StateMachineScene::collapseItem(State* state)
{
    if (!state || !state->isExpanded())
        return;

    state->setExpanded(false);
    d->updateChildItemVisibility(state, false);
}

void StateMachineScene::expandItem(State* state)
{
    if (!state || state->isExpanded())
        return;

    state->setExpanded(true);

    // we delay showing the child items until the layout has been done
    d->m_expandedItem = state;
}

bool StateMachineScene::isItemExpanded(State* state) const
{
    return state ? state->isExpanded() : false;
}

void StateMachineScene::setItemExpanded(State* state, bool expand)
{
    if (expand) {
        expandItem(state);
    } else {
        collapseItem(state);
    }
}

bool StateMachineScene::isItemSelected(Element* item)
{
    if (!stateModel() || !item)
        return false;

    const QModelIndex index = stateModel()->indexForObject(item);
    return selectionModel()->isSelected(index);
}

void StateMachineScene::setItemSelected(Element* item, bool selected)
{
    if (!stateModel() || !item || !item)
        return;

    const QModelIndex index = stateModel()->indexForObject(item);
    selectionModel()->select(index, (selected ? QItemSelectionModel::Select : QItemSelectionModel::Deselect));
}

Element* StateMachineScene::currentItem() const
{
    return currentIndex().data(StateModel::ElementRole).value<Element*>();
}

void StateMachineScene::setCurrentItem(Element* item)
{
    if (!stateModel() || !item || !item)
        return;

    const QModelIndex index = stateModel()->indexForObject(item);
    setCurrentIndex(index);
}

Element* StateMachineScene::currentState()
{
    Element *element = selectionModel()->currentIndex().data(StateModel::ElementRole).value<Element*>();
    if (!element || element->type() == Element::ElementType)
        return nullptr;

    if (element->type() == Element::TransitionType)
        element = static_cast<Transition*>(element)->sourceState();

    QQmlEngine::setObjectOwnership(element, QQmlEngine::CppOwnership);
    return element;
}

StateMachine* StateMachineScene::stateMachine() const
{
    return d->m_stateMachine;
}

void StateMachineScene::setStateMachine(StateMachine* stateMachine)
{
    if (d->m_stateMachine == stateMachine)
        return;

    // reset properties
    setZoom(1.0);

    Q_ASSERT(stateModel());
    stateModel()->setState(stateMachine);

    d->m_stateMachine = stateMachine;
    emit stateMachineChanged(d->m_stateMachine);
}

Layouter* StateMachineScene::layouter() const
{
    return d->m_layouter;
}

void StateMachineScene::setLayouter(Layouter* layouter)
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
}

qreal StateMachineScene::zoom() const
{
    return d->m_zoom;
}

void StateMachineScene::setZoom(qreal zoom)
{
    if (qFuzzyCompare(d->m_zoom, zoom))
        return;

    const auto delta = zoom / d->m_zoom;
    zoomBy(delta);

    d->m_zoom = zoom;
    emit zoomChanged(d->m_zoom);
}


void StateMachineScene::zoomBy(qreal zoom)
{
    auto machine = stateMachine();

    QMatrix matrix;
    matrix.scale(zoom, zoom);

    auto oldState = state();
    setState(RefreshState);

    ElementWalker walker(ElementWalker::PreOrderTraversal);
    walker.walkItems(machine, [&](Element* element) -> ElementWalker::VisitResult {
        const auto pos = element->pos();
        element->setPos({pos.x() * zoom, pos.y() * zoom});

        if (auto state = qobject_cast<State*>(element)) {
            element->setWidth(element->width() * zoom);
            element->setHeight(element->height() * zoom);
        }
        if (auto transition = qobject_cast<Transition*>(element)) {
            transition->setShape(matrix.map(transition->shape()));
        }
        return ElementWalker::RecursiveWalk;
    });

    setState(oldState);
}

void StateMachineScene::layout()
{
    qCDebug(KDSME_VIEW) << Q_FUNC_INFO << d->m_layouter << d->m_stateMachine;

    if (!d->m_layouter || !d->m_stateMachine) {
        return;
    }

    auto oldState = state();
    setState(RefreshState);
    d->m_layouter->layout(d->m_stateMachine, layoutProperties());
    setState(oldState);
}

StateModel* StateMachineScene::stateModel() const
{
    return qobject_cast<StateModel*>(model());
}

void StateMachineScene::setModel(QAbstractItemModel* model)
{
    StateModel* stateModel = qobject_cast<StateModel*>(model);
    if (!stateModel) {
        qWarning() << "Invalid model class type, expected StateModel instance";
    }

    KDSME::AbstractView::setModel(stateModel);
}

void StateMachineScene::Private::updateChildItemVisibility(State* state, bool expand)
{
    if (!state)
        return;

    ElementWalker walker(ElementWalker::PreOrderTraversal);
    walker.walkChildren(state, [&](Element* i) -> ElementWalker::VisitResult {
        if (Transition* transition = qobject_cast<Transition*>(i)) {
            // Avoid hiding transitions from states that are collapsed but still visible
            // which have a sibling state as their target
            auto sourceState = transition->sourceState();
            auto targetState = transition->targetState();
            if (sourceState->isVisible()
                    && (sourceState->parentState() ? sourceState->parentState()->children().contains(targetState) : false)) {
                i->setVisible(true);
                return ElementWalker::RecursiveWalk;
            }
        }

        i->setVisible(expand);
        return ElementWalker::RecursiveWalk;
    });
}

void StateMachineScene::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    AbstractView::currentChanged(current, previous);

    Element* currentItem = current.data(StateModel::ElementRole).value<Element*>();
    Element* previousItem = previous.data(StateModel::ElementRole).value<Element*>();
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

    emit currentItemChanged(currentItem);
}

void StateMachineScene::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    AbstractView::rowsAboutToBeRemoved(parent, start, end);
}

void StateMachineScene::rowsInserted(const QModelIndex& parent, int start, int end)
{
    AbstractView::rowsInserted(parent, start, end);
}

void StateMachineScene::layoutChanged()
{
    AbstractView::layoutChanged();
}
