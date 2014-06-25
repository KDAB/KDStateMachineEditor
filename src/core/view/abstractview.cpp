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

AbstractView::AbstractView(QObject* parent)
    : QObject(parent)
    , m_model(nullptr)
    , m_editTriggers(NoEditTriggers)
    , m_state(NoState)
{
}

QAbstractItemModel* AbstractView::model() const
{
    return m_model;
}

void AbstractView::setModel(QAbstractItemModel* model)
{
    if (m_model == model)
        return;

    if (m_model) {
        disconnect(m_model, &QAbstractItemModel::rowsAboutToBeRemoved,
                   this, &AbstractView::rowsAboutToBeRemoved);
        disconnect(m_model, &QAbstractItemModel::rowsInserted,
                   this, &AbstractView::rowsInserted);
        disconnect(m_model, &QAbstractItemModel::rowsMoved,
                   this, &AbstractView::layoutChanged);
        disconnect(m_model, &QAbstractItemModel::columnsMoved,
                   this, &AbstractView::layoutChanged);
        disconnect(m_model, &QAbstractItemModel::layoutChanged,
                   this, &AbstractView::layoutChanged);
    }

    m_model = model;

    if (m_model) {
        connect(m_model, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, &AbstractView::rowsAboutToBeRemoved);
        connect(m_model, &QAbstractItemModel::rowsInserted,
                this, &AbstractView::rowsInserted);
        connect(m_model, &QAbstractItemModel::rowsMoved,
                this, &AbstractView::layoutChanged);
        connect(m_model, &QAbstractItemModel::columnsMoved,
                this, &AbstractView::layoutChanged);
        connect(m_model, &QAbstractItemModel::layoutChanged,
                this, &AbstractView::layoutChanged);
    }

    QItemSelectionModel *selectionModel = new QItemSelectionModel(m_model, this);
    connect(m_model, SIGNAL(destroyed()), selectionModel, SLOT(deleteLater()));
    setSelectionModel(selectionModel);

    emit modelChanged(m_model);
}

QItemSelectionModel* AbstractView::selectionModel() const
{
    return m_selectionModel;
}

void AbstractView::setSelectionModel(QItemSelectionModel* selectionModel)
{
    Q_ASSERT(selectionModel);
    if (selectionModel->model() != m_model) {
        qWarning("QAbstractItemView::setSelectionModel() failed: "
                 "Trying to set a selection model, which works on "
                 "a different model than the view.");
        return;
    }

    if (m_selectionModel) {
        disconnect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                   this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
        disconnect(m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    }

    m_selectionModel = selectionModel;

    if (m_selectionModel) {
        connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
        connect(m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    }
}

AbstractView::EditTriggers AbstractView::editTriggers() const
{
    return m_editTriggers;
}

void AbstractView::setEditTriggers(AbstractView::EditTriggers triggers)
{
    m_editTriggers = triggers;
}

AbstractView::ViewState AbstractView::state() const
{
    return m_state;
}

void AbstractView::setState(AbstractView::ViewState state)
{
    if (m_state == state)
        return;

    m_state = state;
    emit stateChanged(m_state);
}

void AbstractView::setCurrentIndex(const QModelIndex& index)
{
    if (!m_selectionModel) {
        return;
    }

    m_selectionModel->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
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
