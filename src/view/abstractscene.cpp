/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "abstractscene.h"
#include "abstractscene_p.h"

#include "debug.h"
#include "instantiatorinterface_p.h"

#include <QItemSelectionModel>

using namespace KDSME;

class KDSME::AbstractSceneContextMenuEventPrivate
{
public:
    Element *m_elementUnderCursor = nullptr;
};

AbstractSceneContextMenuEvent::AbstractSceneContextMenuEvent(Reason reason, const QPoint &pos, const QPoint &globalPos, Qt::KeyboardModifiers modifiers, Element *elementUnderCursor) // clazy:exclude=function-args-by-value
    : QContextMenuEvent(reason, pos, globalPos, modifiers)
    , d(new AbstractSceneContextMenuEventPrivate)
{
    d->m_elementUnderCursor = elementUnderCursor;
}

AbstractSceneContextMenuEvent::~AbstractSceneContextMenuEvent()
{
}

Element *AbstractSceneContextMenuEvent::elementUnderCursor() const
{
    return d->m_elementUnderCursor;
}

AbstractScenePrivate::AbstractScenePrivate(AbstractScene *qq)
    : q(qq)
    , m_model(nullptr)
    , m_instantiator(nullptr)
    , m_editTriggers(AbstractScene::NoEditTriggers)
    , m_state(AbstractScene::NoState)
{
}

bool AbstractScene::event(QEvent *event)
{
    if (auto contextMenuEvent = dynamic_cast<AbstractSceneContextMenuEvent *>(event)) {
        switch (d->m_contextMenuPolicy) {
        case Qt::CustomContextMenu:
            event->accept();
            Q_EMIT customContextMenuEvent(contextMenuEvent);
            break;
        default:
            event->ignore();
            break;
        }
    }

    if (event->isAccepted())
        return true;

    return QQuickItem::event(event);
}

AbstractScene::AbstractScene(QQuickItem *parent)
    : QQuickItem(parent)
    , d(new AbstractScenePrivate(this))
{
}

AbstractScene::~AbstractScene() = default; // NOLINT(clang-analyzer-cplusplus.NewDelete)

QAbstractItemModel *AbstractScene::model() const
{
    return d->m_model;
}

void AbstractScene::setModel(QAbstractItemModel *model)
{
    if (d->m_model == model)
        return;

    if (d->m_model) {
        disconnect(d->m_model, &QAbstractItemModel::rowsAboutToBeRemoved,
                   this, &AbstractScene::rowsAboutToBeRemoved);
        disconnect(d->m_model, &QAbstractItemModel::rowsInserted,
                   this, &AbstractScene::rowsInserted);
        disconnect(d->m_model, &QAbstractItemModel::rowsMoved,
                   this, &AbstractScene::layoutChanged);
        disconnect(d->m_model, &QAbstractItemModel::columnsMoved,
                   this, &AbstractScene::layoutChanged);
        disconnect(d->m_model, &QAbstractItemModel::layoutChanged,
                   this, &AbstractScene::layoutChanged);
    }

    d->m_model = model;

    if (d->m_model) {
        connect(d->m_model, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, &AbstractScene::rowsAboutToBeRemoved);
        connect(d->m_model, &QAbstractItemModel::rowsInserted,
                this, &AbstractScene::rowsInserted);
        connect(d->m_model, &QAbstractItemModel::rowsMoved,
                this, &AbstractScene::layoutChanged);
        connect(d->m_model, &QAbstractItemModel::columnsMoved,
                this, &AbstractScene::layoutChanged);
        connect(d->m_model, &QAbstractItemModel::layoutChanged,
                this, &AbstractScene::layoutChanged);
    }

    auto *itemSelectionModel = new QItemSelectionModel(d->m_model, this);
    connect(d->m_model, &QAbstractItemModel::destroyed, itemSelectionModel, &QItemSelectionModel::deleteLater);
    setSelectionModel(itemSelectionModel);

    Q_EMIT modelChanged(d->m_model);
}

QItemSelectionModel *AbstractScene::selectionModel() const
{
    return d->m_selectionModel;
}

void AbstractScene::setSelectionModel(QItemSelectionModel *selectionModel)
{
    Q_ASSERT(selectionModel);
    if (selectionModel && selectionModel->model() != d->m_model) {
        qCWarning(KDSME_VIEW) << "QAbstractItemView::setSelectionModel() failed: "
                                 "Trying to set a selection model, which works on "
                                 "a different model than the view.";
        return;
    }

    if (d->m_selectionModel) {
        disconnect(d->m_selectionModel, &QItemSelectionModel::selectionChanged,
                   this, &AbstractScene::selectionChanged);
        disconnect(d->m_selectionModel, &QItemSelectionModel::currentChanged,
                   this, &AbstractScene::currentChanged);
    }

    d->m_selectionModel = selectionModel;

    if (d->m_selectionModel) {
        connect(d->m_selectionModel, &QItemSelectionModel::selectionChanged,
                this, &AbstractScene::selectionChanged);
        connect(d->m_selectionModel, &QItemSelectionModel::currentChanged,
                this, &AbstractScene::currentChanged);
    }
}

QQuickItem *AbstractScene::instantiator() const
{
    return d->m_instantiator;
}

void AbstractScene::setInstantiator(QQuickItem *instantiator)
{
    if (d->m_instantiator == instantiator)
        return;

    if (!dynamic_cast<InstantiatorInterface *>(instantiator)) {
        qCDebug(KDSME_VIEW) << "Instantiator object must implement InstantiatorInterface";
        return;
    }

    d->m_instantiator = instantiator;
    d->m_instantiator->setParentItem(this);
    Q_EMIT instantiatorChanged(d->m_instantiator);
}

AbstractScene::EditTriggers AbstractScene::editTriggers() const
{
    return d->m_editTriggers;
}

void AbstractScene::setEditTriggers(AbstractScene::EditTriggers triggers)
{
    d->m_editTriggers = triggers;
}

Qt::ContextMenuPolicy AbstractScene::contextMenuPolicy() const
{
    return d->m_contextMenuPolicy;
}

void AbstractScene::setContextMenuPolicy(Qt::ContextMenuPolicy contextMenuPolicy)
{
    if (d->m_contextMenuPolicy == contextMenuPolicy) {
        return;
    }

    d->m_contextMenuPolicy = contextMenuPolicy;
    Q_EMIT contextMenuPolicyChanged(d->m_contextMenuPolicy);
}

QObject *AbstractScene::itemForIndex(const QModelIndex &index) const
{
    const auto *iface = dynamic_cast<InstantiatorInterface *>(d->m_instantiator);
    return iface ? iface->itemForIndex(index) : nullptr;
}

AbstractScene::ViewState AbstractScene::viewState() const
{
    return d->m_state;
}

void AbstractScene::setViewState(AbstractScene::ViewState state)
{
    if (d->m_state == state)
        return;

    d->m_state = state;
    Q_EMIT viewStateChanged(d->m_state);
}

QModelIndex AbstractScene::currentIndex() const
{
    return d->m_selectionModel ? d->m_selectionModel->currentIndex() : QModelIndex();
}

void AbstractScene::setCurrentIndex(const QModelIndex &index)
{
    if (!d->m_selectionModel) {
        return;
    }

    d->m_selectionModel->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
}

void AbstractScene::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

void AbstractScene::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
}

void AbstractScene::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void AbstractScene::rowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void AbstractScene::layoutChanged()
{
}
