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

#ifndef KDSME_UTIL_OBJECTTREEMODEL_H
#define KDSME_UTIL_OBJECTTREEMODEL_H

#include "kdsme_core_export.h"

#include <QAbstractItemModel>

namespace KDSME {

class ObjectTreeModelPrivate;

class KDSME_CORE_EXPORT ObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    class KDSME_CORE_EXPORT AppendOperation // krazy:exclude=dpointer // clazy:exclude=rule-of-three
    {
    public:
        AppendOperation(ObjectTreeModel *model, QObject *parent, int count = 1, int index = -1);
        ~AppendOperation();

    private:
        ObjectTreeModel *m_model;
    };

    class KDSME_CORE_EXPORT RemoveOperation // krazy:exclude=dpointer // clazy:exclude=rule-of-three
    {
    public:
        RemoveOperation(ObjectTreeModel *model, QObject *object);
        ~RemoveOperation();

    private:
        ObjectTreeModel *m_model;
    };

    class KDSME_CORE_EXPORT ResetOperation // krazy:exclude=dpointer // clazy:exclude=rule-of-three
    {
    public:
        explicit ResetOperation(ObjectTreeModel *model);
        ~ResetOperation();

    private:
        ObjectTreeModel *m_model;
    };

    class KDSME_CORE_EXPORT ReparentOperation // krazy:exclude=dpointer // clazy:exclude=rule-of-three
    {
    public:
        ReparentOperation(ObjectTreeModel *model, QObject *object, QObject *newParent);
        ~ReparentOperation();

    private:
        ObjectTreeModel *m_model;
    };

    enum Roles
    {
        ObjectRole = Qt::UserRole + 1, ///< return QObject*
        ObjectIdRole, ///< return quint64
        UserRole = Qt::UserRole + 100
    };

    explicit ObjectTreeModel(QObject *parent = nullptr);
    ~ObjectTreeModel();

    void appendRootObject(QObject *object);
    QList<QObject *> rootObjects() const;
    void setRootObject(QObject *rootObject);
    void setRootObjects(const QList<QObject *> &rootObjects);
    void clear();

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    QModelIndex indexForObject(QObject *object) const;

protected:
    Q_DECLARE_PRIVATE(ObjectTreeModel)
    ObjectTreeModelPrivate *const d_ptr;
};

}

Q_DECLARE_METATYPE(KDSME::ObjectTreeModel *)

#endif
