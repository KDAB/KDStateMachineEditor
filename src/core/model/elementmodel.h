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

#ifndef KDSME_MODEL_ELEMENTMODEL_H
#define KDSME_MODEL_ELEMENTMODEL_H

#include "element.h"
#include "objecttreemodel.h"
#include "kdsme_core_export.h"

#include <QAbstractItemModel>
#include <QObject>
#include <QPointer>
#include <QSortFilterProxyModel>

namespace KDSME {

class StateModel;
class State;

class KDSME_CORE_EXPORT TransitionModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TransitionModel(QObject *parent = nullptr);
    ~TransitionModel();

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT TransitionListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(KDSME::State *state READ state WRITE setState NOTIFY stateChanged)

public:
    enum Role
    {
        ObjectRole = Qt::UserRole + 1,
    };

    enum Column
    {
        NameColumn,
        SourceStateColumn,
        TargetStateColumn,
        _LastColumn
    };

    explicit TransitionListModel(QObject *parent = nullptr);
    ~TransitionListModel();

    State *state() const;
    void setState(State *state);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void stateChanged();

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT StateModel : public ObjectTreeModel
{
    Q_OBJECT
    Q_PROPERTY(KDSME::State *state READ state WRITE setState NOTIFY stateChanged)

public:
    enum Role
    {
        ElementRole = ObjectTreeModel::UserRole + 1, ///< return Element*
        InternalIdRole, ///< return quint64
    };

    explicit StateModel(QObject *parent = nullptr);
    ~StateModel();

    State *state() const;
    void setState(State *state);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

Q_SIGNALS:
    void stateChanged();

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::TransitionModel *)
Q_DECLARE_METATYPE(KDSME::StateModel *)

#endif
