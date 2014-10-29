/*
  abstractview.h

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

#ifndef KDSME_VIEW_ABSTRACTVIEW_H
#define KDSME_VIEW_ABSTRACTVIEW_H

#include "kdsme_core_export.h"

#include <QObject>
#include <QPointer>

class QAbstractItemModel;
class QItemSelection;
class QItemSelectionModel;
class QModelIndex;

namespace KDSME {

/**
 * @brief QAbstractItemView-like class without actual visual representation (i.e. no dependency on QWidget)
 */
class KDSME_CORE_EXPORT AbstractView : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(ViewState state READ state NOTIFY stateChanged FINAL)

public:
    enum EditTrigger {
        NoEditTriggers,
        SelectedClicked
    };
    Q_DECLARE_FLAGS(EditTriggers, EditTrigger)

    enum ViewState {
        NoState,
        ItemLayoutState
    };
    Q_ENUMS(ViewState)

    explicit AbstractView(QObject* parent = nullptr);
    virtual ~AbstractView();

    virtual void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    virtual void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel() const;

    void setEditTriggers(EditTriggers triggers);
    EditTriggers editTriggers() const;

    ViewState state() const;

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

    void setState(ViewState state);

Q_SIGNALS:
    void modelChanged(QAbstractItemModel* model);
    void stateChanged(ViewState state);

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::AbstractView*)
Q_DECLARE_METATYPE(KDSME::AbstractView::ViewState)

#endif // ABSTRACTVIEW_H
