/*
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

#include "abstractscene.h"

#include "debug.h"
#include "instantiatorinterface_p.h"

#include <QItemSelectionModel>

using namespace KDSME;

struct AbstractScene::Private
{
    Private();

    QAbstractItemModel* m_model;
    QPointer<QItemSelectionModel> m_selectionModel;
    QQuickItem* m_instantiator;
    AbstractScene::EditTriggers m_editTriggers;
    AbstractScene::ViewState m_state;
};

AbstractScene::Private::Private()
    : m_model(nullptr)
    , m_instantiator(nullptr)
    , m_editTriggers(NoEditTriggers)
    , m_state(NoState)
{
}

AbstractScene::AbstractScene(QQuickItem* parent)
    : QQuickItem(parent)
    , d(new Private)
{
}

AbstractScene::~AbstractScene()
{
}

QAbstractItemModel* AbstractScene::model() const
{
    return d->m_model;
}

void AbstractScene::setModel(QAbstractItemModel* model)
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

    QItemSelectionModel *selectionModel = new QItemSelectionModel(d->m_model, this);
    connect(d->m_model, SIGNAL(destroyed()), selectionModel, SLOT(deleteLater()));
    setSelectionModel(selectionModel);

    emit modelChanged(d->m_model);
}

QItemSelectionModel* AbstractScene::selectionModel() const
{
    return d->m_selectionModel;
}

void AbstractScene::setSelectionModel(QItemSelectionModel* selectionModel)
{
    Q_ASSERT(selectionModel);
    if (selectionModel->model() != d->m_model) {
        qWarning("QAbstractItemView::setSelectionModel() failed: "
                 "Trying to set a selection model, which works on "
                 "a different model than the view.");
        return;
    }

    if (d->m_selectionModel) {
        disconnect(d->m_selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                   this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
        disconnect(d->m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    }

    d->m_selectionModel = selectionModel;

    if (d->m_selectionModel) {
        connect(d->m_selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
        connect(d->m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    }
}

QQuickItem* AbstractScene::instantiator() const
{
    return d->m_instantiator;
}

void AbstractScene::setInstantiator(QQuickItem* instantiator)
{
    if (d->m_instantiator == instantiator)
        return;

    if (!dynamic_cast<InstantiatorInterface*>(instantiator)) {
        qCDebug(KDSME_VIEW) << "Instantiator object must implement InstantiatorInterface";
        return;
    }

    d->m_instantiator = instantiator;
    d->m_instantiator->setParentItem(this);
    emit instantiatorChanged(d->m_instantiator);
}

AbstractScene::EditTriggers AbstractScene::editTriggers() const
{
    return d->m_editTriggers;
}

void AbstractScene::setEditTriggers(AbstractScene::EditTriggers triggers)
{
    d->m_editTriggers = triggers;
}

QObject* AbstractScene::itemForIndex(const QModelIndex& index) const
{
    auto instantiator = dynamic_cast<InstantiatorInterface*>(d->m_instantiator);
    return instantiator ? instantiator->itemForIndex(index) : nullptr;
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
    emit stateChanged(d->m_state);
}

QModelIndex AbstractScene::currentIndex() const
{
    return d->m_selectionModel ? d->m_selectionModel->currentIndex() : QModelIndex();
}

void AbstractScene::setCurrentIndex(const QModelIndex& index)
{
    if (!d->m_selectionModel) {
        return;
    }

    d->m_selectionModel->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
}

void AbstractScene::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

void AbstractScene::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
}

void AbstractScene::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void AbstractScene::rowsInserted(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void AbstractScene::layoutChanged()
{
}
