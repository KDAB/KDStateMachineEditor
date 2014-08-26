/*
  abstractview.cpp

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

#include "abstractview.h"

#include <QDebug>
#include <QItemSelectionModel>

using namespace KDSME;

struct AbstractView::Private
{
    Private();

    QAbstractItemModel* m_model;
    QPointer<QItemSelectionModel> m_selectionModel;
    AbstractView::EditTriggers m_editTriggers;
    AbstractView::ViewState m_state;
};

AbstractView::Private::Private()
    : m_model(nullptr)
    , m_editTriggers(NoEditTriggers)
    , m_state(NoState)
{

}

AbstractView::AbstractView(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

AbstractView::~AbstractView()
{
}

QAbstractItemModel* AbstractView::model() const
{
    return d->m_model;
}

void AbstractView::setModel(QAbstractItemModel* model)
{
    if (d->m_model == model)
        return;

    if (d->m_model) {
        disconnect(d->m_model, &QAbstractItemModel::rowsAboutToBeRemoved,
                   this, &AbstractView::rowsAboutToBeRemoved);
        disconnect(d->m_model, &QAbstractItemModel::rowsInserted,
                   this, &AbstractView::rowsInserted);
        disconnect(d->m_model, &QAbstractItemModel::rowsMoved,
                   this, &AbstractView::layoutChanged);
        disconnect(d->m_model, &QAbstractItemModel::columnsMoved,
                   this, &AbstractView::layoutChanged);
        disconnect(d->m_model, &QAbstractItemModel::layoutChanged,
                   this, &AbstractView::layoutChanged);
    }

    d->m_model = model;

    if (d->m_model) {
        connect(d->m_model, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, &AbstractView::rowsAboutToBeRemoved);
        connect(d->m_model, &QAbstractItemModel::rowsInserted,
                this, &AbstractView::rowsInserted);
        connect(d->m_model, &QAbstractItemModel::rowsMoved,
                this, &AbstractView::layoutChanged);
        connect(d->m_model, &QAbstractItemModel::columnsMoved,
                this, &AbstractView::layoutChanged);
        connect(d->m_model, &QAbstractItemModel::layoutChanged,
                this, &AbstractView::layoutChanged);
    }

    QItemSelectionModel *selectionModel = new QItemSelectionModel(d->m_model, this);
    connect(d->m_model, SIGNAL(destroyed()), selectionModel, SLOT(deleteLater()));
    setSelectionModel(selectionModel);

    emit modelChanged(d->m_model);
}

QItemSelectionModel* AbstractView::selectionModel() const
{
    return d->m_selectionModel;
}

void AbstractView::setSelectionModel(QItemSelectionModel* selectionModel)
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

AbstractView::EditTriggers AbstractView::editTriggers() const
{
    return d->m_editTriggers;
}

void AbstractView::setEditTriggers(AbstractView::EditTriggers triggers)
{
    d->m_editTriggers = triggers;
}

AbstractView::ViewState AbstractView::state() const
{
    return d->m_state;
}

void AbstractView::setState(AbstractView::ViewState state)
{
    if (d->m_state == state)
        return;

    d->m_state = state;
    emit stateChanged(d->m_state);
}

void AbstractView::setCurrentIndex(const QModelIndex& index)
{
    if (!d->m_selectionModel) {
        return;
    }

    d->m_selectionModel->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
}

void AbstractView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

void AbstractView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
}

void AbstractView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void AbstractView::rowsInserted(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void AbstractView::layoutChanged()
{
}
