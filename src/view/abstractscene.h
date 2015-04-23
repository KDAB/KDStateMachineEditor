/*
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

#ifndef KDSME_VIEW_ABSTRACTSCENE_H
#define KDSME_VIEW_ABSTRACTSCENE_H

#include "kdsme_view_export.h"

#include <QQuickItem>
#include <QPointer>

class QAbstractItemModel;
class QItemSelection;
class QItemSelectionModel;
class QModelIndex;

namespace KDSME {

class KDSME_VIEW_EXPORT AbstractScene : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QQuickItem* instantiator READ instantiator WRITE setInstantiator NOTIFY instantiatorChanged)
    Q_PROPERTY(ViewState viewState READ viewState NOTIFY stateChanged FINAL)

public:
    enum EditTrigger {
        NoEditTriggers,
        SelectedClicked
    };
    Q_DECLARE_FLAGS(EditTriggers, EditTrigger)

    enum ViewState {
        NoState,
        RefreshState,
    };
    Q_ENUMS(ViewState)

    explicit AbstractScene(QQuickItem* parent = nullptr);
    virtual ~AbstractScene();

    virtual void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    virtual void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel() const;

    QQuickItem* instantiator() const;
    void setInstantiator(QQuickItem* instantiator);

    void setEditTriggers(EditTriggers triggers);
    EditTriggers editTriggers() const;

    QObject* itemForIndex(const QModelIndex& index) const;

    ViewState viewState() const;

    QModelIndex currentIndex() const;

public Q_SLOTS:
    void setCurrentIndex(const QModelIndex& index);

protected Q_SLOTS:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous);

    virtual void rowsInserted(const QModelIndex& parent, int start, int end);
    virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);

    /**
     * Called whenever the model's contents layout changed
     * (for example when indices got moved around)
     */
    virtual void layoutChanged();

    void setViewState(ViewState state);

Q_SIGNALS:
    void modelChanged(QAbstractItemModel* model);
    void stateChanged(ViewState state);
    void instantiatorChanged(QObject* instantiator);

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::AbstractScene*)
Q_DECLARE_METATYPE(KDSME::AbstractScene::ViewState)

#endif // ABSTRACTVIEW_H
