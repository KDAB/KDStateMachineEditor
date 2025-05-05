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

#include "objecttreemodel.h"

#include "debug.h"

using namespace KDSME;

namespace KDSME {

class ObjectTreeModelPrivate
{
    explicit ObjectTreeModelPrivate(ObjectTreeModel *qq)
        : q_ptr(qq)
    {
    }

    Q_DECLARE_PUBLIC(ObjectTreeModel)
    ObjectTreeModel *const q_ptr;
    QList<QObject *> m_rootObjects;

    QList<QObject *> children(QObject *parent) const;

    [[nodiscard]] QObject *mapModelIndex2QObject(const QModelIndex &index) const;
    QModelIndex indexForObject(QObject *object) const;
};

}

QList<QObject *> ObjectTreeModelPrivate::children(QObject *parent) const
{
    if (!parent) {
        return m_rootObjects;
    }
    return parent->children();
}

QObject *ObjectTreeModelPrivate::mapModelIndex2QObject(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return nullptr;
    }

    auto *parent = reinterpret_cast<QObject *>(index.internalPointer());
    if (!parent) {
        return m_rootObjects[index.row()];
    }
    QObjectList c = children(parent);
    return c[index.row()];
}

QModelIndex ObjectTreeModelPrivate::indexForObject(QObject *object) const
{
    if (!object) {
        return QModelIndex();
    }

    Q_Q(const ObjectTreeModel);
    int row = static_cast<int>(m_rootObjects.indexOf(object));
    if (row != -1) {
        return q->index(row, 0, QModelIndex());
    }

    row = static_cast<int>(children(object->parent()).indexOf(object));
    if (row == -1) {
        return {};
    }
    return q->index(row, 0, indexForObject(object->parent()));
}

ObjectTreeModel::AppendOperation::AppendOperation(ObjectTreeModel *model, QObject *parent, int count, int index)
    : m_model(model)
{
    Q_ASSERT(m_model);
    const QModelIndex parentIndex = m_model->indexForObject(parent);
    Q_ASSERT(parentIndex.isValid());
    const int first = index >= 0 ? index : m_model->rowCount(parentIndex);
    const int last = first + count - 1;
    Q_ASSERT(first >= 0 && last >= 0);
    Q_ASSERT(first <= last);

    m_model->beginInsertRows(parentIndex, first, last);
}

ObjectTreeModel::AppendOperation::~AppendOperation()
{
    m_model->endInsertRows();
}

ObjectTreeModel::RemoveOperation::RemoveOperation(ObjectTreeModel *model, QObject *object)
    : m_model(model)
{
    Q_ASSERT(m_model);
    Q_ASSERT(object);
    Q_ASSERT(object->parent());
    Q_ASSERT(!m_model->rootObjects().contains(object));
    const QModelIndex indexObj = m_model->indexForObject(object);
    const QModelIndex parentIndex = m_model->indexForObject(object->parent());
    m_model->beginRemoveRows(parentIndex, indexObj.row(), indexObj.row());
}

ObjectTreeModel::RemoveOperation::~RemoveOperation()
{
    m_model->endRemoveRows();
}

ObjectTreeModel::ResetOperation::ResetOperation(ObjectTreeModel *model)
    : m_model(model)
{
    if (m_model) {
        m_model->beginResetModel();
    }
}

ObjectTreeModel::ResetOperation::~ResetOperation()
{
    if (m_model) {
        m_model->endResetModel();
    }
}

ObjectTreeModel::ReparentOperation::ReparentOperation(ObjectTreeModel *model, QObject *object, QObject *newParent)
    : m_model(model)
{
    // some sanity checks
    if (!object || (object->parent() == newParent) || (object == newParent)) {
        m_model = nullptr;
    }

    if (m_model) {
        const QModelIndex indexObj = m_model->indexForObject(object);
        QObject *parentObj = object->parent(); // cppcheck-suppress constVariablePointer
        const QModelIndex parentIndex = m_model->indexForObject(parentObj);
        const QModelIndex destinationParentIndex = m_model->indexForObject(newParent);
        Q_ASSERT(destinationParentIndex.isValid());
        const bool success = m_model->beginMoveRows(parentIndex, indexObj.row(), indexObj.row(), destinationParentIndex, m_model->rowCount(destinationParentIndex));
        Q_ASSERT(success);
        Q_UNUSED(success);
    }
}

ObjectTreeModel::ReparentOperation::~ReparentOperation()
{
    if (m_model) {
        m_model->endMoveRows();
    }
}

ObjectTreeModel::ObjectTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new ObjectTreeModelPrivate(this))
{
}

ObjectTreeModel::~ObjectTreeModel()
{
    delete d_ptr;
}

QHash<int, QByteArray> ObjectTreeModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(ObjectRole, "object");
    roleNames.insert(ObjectIdRole, "objectid");
    return roleNames;
}

void ObjectTreeModel::appendRootObject(QObject *object)
{
    Q_D(ObjectTreeModel);
    if (!object || d->m_rootObjects.contains(object)) {
        return;
    }

    const int row = static_cast<int>(d->m_rootObjects.count());
    beginInsertRows({}, row, row);
    d->m_rootObjects << object;
    endInsertRows();
}

QList<QObject *> ObjectTreeModel::rootObjects() const
{
    Q_D(const ObjectTreeModel);
    return d->m_rootObjects;
}

void ObjectTreeModel::setRootObject(QObject *rootObject)
{
    setRootObjects(QList<QObject *>() << rootObject);
}

void ObjectTreeModel::setRootObjects(const QList<QObject *> &rootObjects)
{
    Q_D(ObjectTreeModel);
    beginResetModel();
    d->m_rootObjects.clear();
    for (QObject *object : rootObjects) { // cppcheck-suppress constVariablePointer
        if (object)
            d->m_rootObjects << object;
    }
    endResetModel();
}

void ObjectTreeModel::clear()
{
    Q_D(ObjectTreeModel);
    beginResetModel();
    d->m_rootObjects.clear();
    endResetModel();
}

QVariant ObjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    Q_D(const ObjectTreeModel);
    QObject *obj = d->mapModelIndex2QObject(index);
    Q_ASSERT(obj);
    switch (role) {
    case Qt::DisplayRole:
        return QString { u"0x" + QString::number(reinterpret_cast<quint64>(obj), 16) };
    case ObjectRole:
        return QVariant::fromValue(obj);
    case ObjectIdRole:
        return reinterpret_cast<quint64>(obj);
    default:
        return {};
    }
}

int ObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const ObjectTreeModel);
    return static_cast<int>(d->children(d->mapModelIndex2QObject(parent)).count());
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const ObjectTreeModel);
    if (row < 0 || column < 0 || column >= columnCount(parent)) {
        return QModelIndex();
    }

    if (!parent.isValid() && row < rowCount()) {
        return createIndex(row, column, nullptr);
    }

    QObject *parentObject = d->mapModelIndex2QObject(parent);
    if (!parentObject)
        return QModelIndex();
    const QObjectList c = d->children(parentObject);
    if (row >= c.size()) {
        return QModelIndex();
    }

    return createIndex(row, column, parentObject);
}

QModelIndex ObjectTreeModel::indexForObject(QObject *object) const
{
    Q_D(const ObjectTreeModel);
    return d->indexForObject(object);
}

QModelIndex ObjectTreeModel::parent(const QModelIndex &index) const
{
    Q_D(const ObjectTreeModel);
    if (!index.isValid()) {
        return QModelIndex();
    }

    QObject *object = d->mapModelIndex2QObject(index);
    QObject *parent = object->parent();
    if (!parent) {
        return QModelIndex();
    }

    QObject *grandParent = parent->parent();
    const int row = static_cast<int>(d->children(grandParent).indexOf(parent));
    return createIndex(row, 0, grandParent);
}
