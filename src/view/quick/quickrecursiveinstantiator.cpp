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

#include "quickrecursiveinstantiator_p.h"

#include "objecttreemodel.h"

#include <QAbstractItemModel>
#include "debug.h"
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>

using namespace KDSME;

QuickRecursiveInstantiator::QuickRecursiveInstantiator(QQuickItem *parent)
    : QQuickItem(parent)
    , m_model(nullptr)
    , m_delegate(nullptr)
{
}

QList<QObject *> QuickRecursiveInstantiator::rootItems() const
{
    return m_rootItems;
}

QAbstractItemModel *QuickRecursiveInstantiator::model() const
{
    return m_model;
}

void QuickRecursiveInstantiator::setModel(QAbstractItemModel *model)
{
    if (m_model == model) {
        return;
    }
    if (m_model) {
        disconnect(m_model, &QAbstractItemModel::modelReset, this, &QuickRecursiveInstantiator::modelReset);
        disconnect(m_model, &QAbstractItemModel::rowsInserted, this, &QuickRecursiveInstantiator::rowsInserted);
        disconnect(m_model, &QAbstractItemModel::rowsRemoved, this, &QuickRecursiveInstantiator::rowsAboutToBeRemoved);
        disconnect(m_model, &QAbstractItemModel::destroyed, this, &QuickRecursiveInstantiator::modelDestroyed);
    }
    m_model = model;
    if (m_model) {
        connect(m_model, &QAbstractItemModel::modelReset, this, &QuickRecursiveInstantiator::modelReset);
        connect(m_model, &QAbstractItemModel::rowsInserted, this, &QuickRecursiveInstantiator::rowsInserted);
        connect(m_model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &QuickRecursiveInstantiator::rowsAboutToBeRemoved);
        connect(m_model, &QAbstractItemModel::destroyed, this, &QuickRecursiveInstantiator::modelDestroyed);
    }
    Q_EMIT modelChanged(m_model);
}

QQmlComponent *QuickRecursiveInstantiator::delegate() const
{
    return m_delegate;
}

void QuickRecursiveInstantiator::setDelegate(QQmlComponent *delegate)
{
    if (m_delegate == delegate) {
        return;
    }

    m_delegate = delegate;
    Q_EMIT delegateChanged(m_delegate);
}

QObject *QuickRecursiveInstantiator::itemForIndex(const QModelIndex &index) const
{
    return m_createdItems.value(index);
}

void QuickRecursiveInstantiator::modelReset()
{
    Q_ASSERT(m_model);

    std::for_each(m_rootItems.begin(), m_rootItems.end(), [](QObject *obj) { obj->deleteLater(); });
    m_rootItems.clear();

    for (int i = 0; i < m_model->rowCount(); ++i) {
        auto rootIndex = m_model->index(0, 0);
        m_rootItems << createItems(rootIndex, this);
    }
}

void QuickRecursiveInstantiator::modelDestroyed()
{
    // qDeleteAll(m_createdItems.values());
    m_createdItems.clear();
    qDeleteAll(m_rootItems);
    m_rootItems.clear();

    m_model = nullptr;
}

QObject *QuickRecursiveInstantiator::createItems(const QModelIndex &index, QObject *parent)
{
    Q_ASSERT(m_delegate);
    Q_ASSERT(m_model);
    Q_ASSERT(index.isValid());

    auto object = index.data(ObjectTreeModel::ObjectRole).value<QObject *>();
    Q_ASSERT(object);
    auto creationContext = m_delegate->creationContext();
    auto context = new QQmlContext(creationContext ? creationContext : qmlContext(this));
    context->setContextProperty(QStringLiteral("object"), object);

    auto createdObject = m_delegate->create(context);
    createdObject->setParent(parent);
    context->setParent(createdObject);
    if (auto quickItem = qobject_cast<QQuickItem *>(createdObject)) {
        if (auto quickParentItem = qobject_cast<QQuickItem *>(parent)) {
            quickItem->setParentItem(quickParentItem);
        } else {
            quickItem->setParentItem(this);
        }
    }

    // register
    m_createdItems[QPersistentModelIndex(index)] = createdObject;

    // create items for child indices recursively
    for (int i = 0; i < m_model->rowCount(index); ++i) {
        auto childIndex = m_model->index(i, 0, index);
        createItems(childIndex, createdObject);
    }

    return createdObject;
}

void QuickRecursiveInstantiator::removeItems(const QModelIndex &index, QObject *parent)
{
    Q_UNUSED(parent);
    Q_ASSERT(m_createdItems.contains(index));
    auto createdObject = m_createdItems.take(index);
    Q_ASSERT(createdObject);
    qDebug() << createdObject << index;

    createdObject->deleteLater();
}

void QuickRecursiveInstantiator::rowsInserted(const QModelIndex &parent, int first, int last)
{
    auto parentItem = itemForIndex(parent);
    Q_ASSERT(parentItem);
    for (int i = first; i <= last; ++i) {
        const auto currentIndex = m_model->index(i, 0, parent);
        createItems(currentIndex, parentItem);
    }
}

void QuickRecursiveInstantiator::rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    auto parentItem = itemForIndex(parent);
    Q_ASSERT(parentItem);
    for (int i = first; i <= last; ++i) {
        const auto currentIndex = m_model->index(i, 0, parent);
        removeItems(currentIndex, parentItem);
    }
}
