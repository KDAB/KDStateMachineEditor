/*
  view.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include <config-kdsme.h>

#include "statemachinescene.h"
#include "statemachinescene_p.h"

#include "debug.h"
#include "state.h"
#include "transition.h"
#include "layouter.h"
#include "layoutproperties.h"
#include "layoututils.h"
#include "elementmodel.h"
#include "elementwalker.h"
#include "objecthelper.h"

#if HAVE_GRAPHVIZ
#include "graphvizlayout/graphvizlayouter.h"
#else
#include "layerwiselayouter.h"
#endif

#include <QDir>
#include <QElapsedTimer>
#include <QItemSelectionModel>
#include <QMatrix>
#include <QPainterPath>
#include <QSortFilterProxyModel>
#include <QQmlEngine>

using namespace KDSME;

StateMachineScene::Private::Private(StateMachineScene* view)
    : q(view)
    , m_rootState(nullptr)
#if HAVE_GRAPHVIZ
    , m_layouter(new GraphvizLayouter(q))
#else
    , m_layouter(new LayerwiseLayouter(q))
#endif
    , m_properties(new LayoutProperties(q))
    , m_zoom(1.0)
    , m_maximumDepth(3)
{
}

StateMachineScene::StateMachineScene(QQuickItem* parent)
    : AbstractScene(parent)
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
    if (!state)
        return;

    state->setExpanded(false);
    d->updateChildItemVisibility(state, false);
}

void StateMachineScene::expandItem(State* state)
{
    if (!state)
        return;

    state->setExpanded(true);
    d->updateChildItemVisibility(state, true);
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
    if (!stateModel() || !item)
        return;

    const QModelIndex index = stateModel()->indexForObject(item);
    setCurrentIndex(index);
}

Element* StateMachineScene::currentState() const
{
    Element *element = selectionModel()->currentIndex().data(StateModel::ElementRole).value<Element*>();
    if (!element || element->type() == Element::ElementType)
        return nullptr;

    if (element->type() == Element::SignalTransitionType || element->type() == Element::TimeoutTransitionType)
        element = static_cast<Transition*>(element)->sourceState();

    QQmlEngine::setObjectOwnership(element, QQmlEngine::CppOwnership);
    return element;
}

State* StateMachineScene::rootState() const
{
    return d->m_rootState;
}

void StateMachineScene::setRootState(State* rootState)
{
    if (d->m_rootState == rootState)
        return;

    // reset properties
    setZoom(1.0);
    setMaximumDepth(3);

    Q_ASSERT(stateModel());
    stateModel()->setState(rootState);

    d->m_rootState = rootState;
    emit rootStateChanged(d->m_rootState);

    d->updateItemVisibilities();
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
    d->zoomByInternal(delta);

    d->m_zoom = zoom;
    emit zoomChanged(d->m_zoom);
}

void StateMachineScene::zoomBy(qreal scale)
{
    setZoom(d->m_zoom * scale);
}

int StateMachineScene::maximumDepth() const
{
    return d->m_maximumDepth;
}

void StateMachineScene::setMaximumDepth(int maximumDepth)
{
    if (maximumDepth <= 0 || d->m_maximumDepth == maximumDepth)
        return;

    d->m_maximumDepth = maximumDepth;
    emit maximumDepthChanged(d->m_maximumDepth);

    auto oldViewState = viewState();
    setViewState(RefreshState);

    d->updateItemVisibilities();
    layout();

    setViewState(oldViewState);
}

void StateMachineScene::Private::zoomByInternal(qreal scale)
{
    auto root = q->rootState();

    QMatrix matrix;
    matrix.scale(scale, scale);

    auto oldViewState = q->viewState();
    q->setViewState(RefreshState);

    ElementWalker walker(ElementWalker::PreOrderTraversal);
    walker.walkItems(root, [&](Element* element) -> ElementWalker::VisitResult {
        element->setPos(matrix.map(element->pos()));
        element->setWidth(element->width() * scale);
        element->setHeight(element->height() * scale);
        if (auto transition = qobject_cast<Transition*>(element)) {
            transition->setShape(matrix.map(transition->shape()));
        }
        return ElementWalker::RecursiveWalk;
    });

    q->setViewState(oldViewState);
}

void StateMachineScene::layout()
{
    qCDebug(KDSME_VIEW) << d->m_layouter << d->m_rootState;

    if (!d->m_layouter || !d->m_rootState) {
        return;
    }

    auto oldViewState = viewState();
    setViewState(RefreshState);

    // reset
    setZoom(1.0);

    QElapsedTimer timer;
    timer.start();

    d->m_layouter->layout(d->m_rootState, layoutProperties());

    qCDebug(KDSME_VIEW) << "Layouting took" << timer.elapsed() << "ms";

    setViewState(oldViewState);
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
        return;
    }

    KDSME::AbstractScene::setModel(stateModel);
}

void StateMachineScene::Private::updateItemVisibilities()
{
    ElementWalker walker(ElementWalker::PreOrderTraversal);
    walker.walkItems(m_rootState, [&](Element* element) -> ElementWalker::VisitResult {
        if (auto state = qobject_cast<State*>(element)) {
            const bool expand = (m_maximumDepth > 0 ? ObjectHelper::depth(m_rootState, state) < m_maximumDepth : true);

            q->setItemExpanded(state, expand);
        }

        return ElementWalker::RecursiveWalk;
    });
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
    AbstractScene::currentChanged(current, previous);

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
    AbstractScene::rowsAboutToBeRemoved(parent, start, end);
}

void StateMachineScene::rowsInserted(const QModelIndex& parent, int start, int end)
{
    AbstractScene::rowsInserted(parent, start, end);
}

void StateMachineScene::layoutChanged()
{
    AbstractScene::layoutChanged();
}
