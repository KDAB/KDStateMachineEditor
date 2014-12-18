/*
  quickrecursiveinstantiator.h

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

#ifndef KDSME_QUICK_QUICKRECURSIVEINSTANTIATOR_H
#define KDSME_QUICK_QUICKRECURSIVEINSTANTIATOR_H

#include "abstractscene.h"

#include <QPersistentModelIndex>
#include <QQuickItem>

class QAbstractItemModel;
class QQmlContext;

class QuickRecursiveInstantiator : public QQuickItem, public KDSME::InstantiatorInterface
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")

public:
    explicit QuickRecursiveInstantiator(QQuickItem* parent = 0);

    QAbstractItemModel* model() const;
    virtual void setModel(QAbstractItemModel* model);

    QQmlComponent* delegate() const;
    void setDelegate(QQmlComponent* delegate);

    virtual QObject* itemForIndex(const QModelIndex& index) const override;

    QList<QObject*> rootItems() const;

private Q_SLOTS:
    void modelReset();
    void modelDestroyed();
    void rowsInserted(const QModelIndex& parent, int row, int column);
    void rowsRemoved(const QModelIndex& parent, int row, int column);

Q_SIGNALS:
    void modelChanged(QAbstractItemModel* model);
    void delegateChanged(QQmlComponent* delegate);

private:
    QObject* createItems(const QModelIndex& index, QObject* parent);

    QAbstractItemModel* m_model;
    QQmlComponent* m_delegate;
    QHash<QPersistentModelIndex, QObject*> m_createdItems;

    QList<QObject*> m_rootItems;
};

#endif // KDSME_QUICK_QUICKRECURSIVEINSTANTIATOR_H
