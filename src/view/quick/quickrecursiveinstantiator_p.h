/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_QUICK_QUICKRECURSIVEINSTANTIATOR_P_H
#define KDSME_QUICK_QUICKRECURSIVEINSTANTIATOR_P_H

#include "instantiatorinterface_p.h"

#include <QPersistentModelIndex>
#include <QQuickItem>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QQmlContext;
QT_END_NAMESPACE

class QuickRecursiveInstantiator : public QQuickItem, public InstantiatorInterface
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")

public:
    explicit QuickRecursiveInstantiator(QQuickItem *parent = nullptr);

    QAbstractItemModel *model() const;
    virtual void setModel(QAbstractItemModel *model);

    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *delegate);

    QObject *itemForIndex(const QModelIndex &index) const override;

    QList<QObject *> rootItems() const;

private Q_SLOTS:
    void modelReset();
    void modelDestroyed();
    void rowsInserted(const QModelIndex &parent, int first, int last);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);

Q_SIGNALS:
    void modelChanged(QAbstractItemModel *model);
    void delegateChanged(QQmlComponent *delegate);

private:
    QObject *createItems(const QModelIndex &index, QObject *parent);
    void removeItems(const QModelIndex &index, QObject *parent);

    QAbstractItemModel *m_model;
    QQmlComponent *m_delegate;
    QHash<QPersistentModelIndex, QObject *> m_createdItems;

    QList<QObject *> m_rootItems;
};

#endif // KDSME_QUICK_QUICKRECURSIVEINSTANTIATOR_P_H
