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

#include "elementmodel.h"

#include "objecthelper.h"
#include "kdsmeconstants.h"
#include "state.h"
#include "transition.h"

#include <QAbstractTransition>
#include "debug.h"
#include <QVariant>

using namespace KDSME;

namespace {

Qt::ItemFlags toItemFlags(Element::Flags flags)
{
    Qt::ItemFlags result;
    if (flags.testFlag(Element::ElementIsDragEnabled))
        result |= Qt::ItemIsDragEnabled;
    if (flags.testFlag(Element::ElementIsSelectable))
        result |= Qt::ItemIsSelectable;
    if (flags.testFlag(Element::ElementIsEditable))
        result |= Qt::ItemIsEditable;
    return result;
}

}

struct StateModel::Private
{
    Private();
};

StateModel::Private::Private()
{
}

StateModel::StateModel(QObject *parent)
    : ObjectTreeModel(parent)
    , d(new Private)
{
}

StateModel::~StateModel()
{
}

State *StateModel::state() const
{
    return qobject_cast<State *>(rootObjects().value(0));
}

void StateModel::setState(State *state)
{
    setRootObject(state);
    Q_EMIT stateChanged();
}

QVariant StateModel::data(const QModelIndex &index, int role) const
{
    auto *element = qobject_cast<Element *>(ObjectTreeModel::data(index, ObjectRole).value<QObject *>());
    if (!element) {
        return ObjectTreeModel::data(index, role);
    }

    switch (role) {
    case Qt::DisplayRole:
        return element->toDisplayString();
    case ElementRole:
        return QVariant::fromValue<Element *>(element);
    case InternalIdRole:
        return element->internalId();
    case Qt::EditRole:
        return element->label();
    }

    return ObjectTreeModel::data(index, role);
}

QVariant StateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
        default:
            return tr("State");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags StateModel::flags(const QModelIndex &index) const
{
    const Element *element = index.data(ElementRole).value<Element *>();
    Q_ASSERT(element);
    if (!element) {
        return QAbstractItemModel::flags(index);
    }

    return toItemFlags(element->flags()) | Qt::ItemIsEnabled;
}

struct TransitionModel::Private
{
};

TransitionModel::TransitionModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d(new Private)
{
}

TransitionModel::~TransitionModel()
{
}

void TransitionModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (!sourceModel) {
        QSortFilterProxyModel::setSourceModel(nullptr);
        return;
    }

    auto *model = qobject_cast<StateModel *>(sourceModel);
    if (!model) {
        qCWarning(KDSME_CORE) << "called with invalid model instance:" << model;
        return;
    }

    QSortFilterProxyModel::setSourceModel(sourceModel);
}

bool TransitionModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_row);

    const auto *object = source_parent.data(StateModel::ObjectRole).value<QObject *>();
    const auto *transition = qobject_cast<const Transition *>(object);

    return transition != nullptr;
}

struct TransitionListModel::Private
{
    Private();

    State *m_state;
    QList<Transition *> m_transitions;
};

TransitionListModel::Private::Private()
    : m_state(nullptr)
{
}

TransitionListModel::TransitionListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

TransitionListModel::~TransitionListModel()
{
}

int TransitionListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(d->m_transitions.size());
}

int TransitionListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _LastColumn;
}

QVariant TransitionListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= rowCount()) {
        return QVariant();
    }

    Transition *transition = d->m_transitions[index.row()];
    Q_ASSERT(transition);
    if (role == Qt::DisplayRole) {
        const int column = index.column();
        switch (column) {
        case NameColumn:
            return transition->toDisplayString();
        case SourceStateColumn:
            return (transition->sourceState() ? transition->sourceState()->toDisplayString() : QStringLiteral("0x0"));
        case TargetStateColumn:
            return (transition->targetState() ? transition->targetState()->toDisplayString() : QStringLiteral("0x0"));
        default:
            return QVariant();
        }
    } else if (role == ObjectRole) {
        return QVariant::fromValue<Transition *>(transition);
    }
    return QVariant();
}

QVariant TransitionListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case NameColumn:
            return tr("Transition");
        case SourceStateColumn:
            return tr("Source State");
        case TargetStateColumn:
            return tr("Target State");
        default:
            return QVariant();
        }
    }
    return QAbstractListModel::headerData(section, orientation, role);
}

QHash<int, QByteArray> TransitionListModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(ObjectRole, "object");
    return roleNames;
}

State *TransitionListModel::state() const
{
    return d->m_state;
}

void TransitionListModel::setState(State *state)
{
    beginResetModel();
    d->m_state = state;
    d->m_transitions = (state ? state->findChildren<Transition *>() : QList<Transition *>());
    // TODO: Track updates to object (newly created/removed transitions)?
    endResetModel();

    Q_EMIT stateChanged();
}

#include "moc_elementmodel.cpp"
