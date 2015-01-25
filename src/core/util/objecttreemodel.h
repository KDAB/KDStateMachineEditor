/*
  objecttreemodel.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
    class KDSME_CORE_EXPORT AppendOperation //krazy:exclude=dpointer
    {
    public:
        AppendOperation(ObjectTreeModel* model, QObject* parent, int count = 1, int index = -1);
        ~AppendOperation();

    private:
        ObjectTreeModel* m_model;
    };

    class KDSME_CORE_EXPORT RemoveOperation //krazy:exclude=dpointer
    {
    public:
        RemoveOperation(ObjectTreeModel* model, QObject* object);
        ~RemoveOperation();

    private:
        ObjectTreeModel* m_model;
    };

    class KDSME_CORE_EXPORT ResetOperation //krazy:exclude=dpointer
    {
    public:
        explicit ResetOperation(ObjectTreeModel* model);
        ~ResetOperation();

    private:
        ObjectTreeModel* m_model;
    };

    class KDSME_CORE_EXPORT ReparentOperation //krazy:exclude=dpointer
    {
    public:
        ReparentOperation(ObjectTreeModel* model, QObject* object, QObject* newParent);
        ~ReparentOperation();

    private:
        ObjectTreeModel* m_model;
    };

    enum Roles {
        ObjectRole = Qt::UserRole + 1,
        UserRole = Qt::UserRole + 100
    };

    explicit ObjectTreeModel(QObject *parent = nullptr);
    ~ObjectTreeModel();

    void appendRootObject(QObject* object);
    QList<QObject*> rootObjects() const;
    void setRootObject(QObject* rootObject);
    void setRootObjects(const QList<QObject*>& rootObjects);
    void clear();

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &index) const override;
    virtual QHash< int, QByteArray > roleNames() const override;

    QModelIndex indexForObject(QObject* object) const;

  protected:
    Q_DECLARE_PRIVATE(ObjectTreeModel)
    ObjectTreeModelPrivate * const d_ptr;
};

}

Q_DECLARE_METATYPE(KDSME::ObjectTreeModel*)

#endif
