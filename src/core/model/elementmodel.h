/*
  elementmodel.h

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

#ifndef KDSME_MODEL_ELEMENTMODEL_H
#define KDSME_MODEL_ELEMENTMODEL_H

#include "element.h"
#include "objecttreemodel.h"
#include "kdsme_core_export.h"

#include <QAbstractItemModel>
#include <QDebug>
#include <QObject>
#include <QPointer>
#include <QSortFilterProxyModel>

class QAbstractState;
class QDebug;
class QState;
class QStateMachine;

namespace KDSME {

class CommandController;
class StateModel;
class State;

class KDSME_CORE_EXPORT TransitionModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TransitionModel(QObject* parent = 0);
    virtual ~TransitionModel();

    virtual void setSourceModel(QAbstractItemModel* sourceModel);

    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT TransitionListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(KDSME::State* state READ state WRITE setState)

public:
    enum Role {
        ObjectRole = Qt::UserRole + 1,
    };

    enum Column {
        NameColumn,
        SourceStateColumn,
        TargetStateColumn,
        _LastColumn
    };

    explicit TransitionListModel(QObject* parent = 0);
    virtual ~TransitionListModel();

    State* state() const;
    void setState(State* list);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QHash< int, QByteArray > roleNames() const Q_DECL_OVERRIDE;

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT StateModel : public ObjectTreeModel
{
    Q_OBJECT
    Q_PROPERTY(KDSME::State* state READ state WRITE setState)

public:
    enum Role {
        ElementRole = ObjectTreeModel::UserRole + 1
    };

    explicit StateModel(QObject* parent = 0);
    virtual ~StateModel();

    State* state() const;
    void setState(State* state);

    void setCommandController(CommandController *cmdController);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::TransitionModel*)
Q_DECLARE_METATYPE(KDSME::StateModel*)

#endif
