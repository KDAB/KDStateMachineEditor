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

#ifndef KDSME_VIEW_ABSTRACTSCENE_H
#define KDSME_VIEW_ABSTRACTSCENE_H

#include "kdsme_view_export.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QItemSelectionModel;
class QModelIndex;
QT_END_NAMESPACE

namespace KDSME {

class AbstractScenePrivate;
class AbstractSceneContextMenuEventPrivate;
class Element;

class KDSME_VIEW_EXPORT AbstractSceneContextMenuEvent : public QContextMenuEvent
{
public:
    AbstractSceneContextMenuEvent(Reason reason, const QPoint &pos, const QPoint &globalPos, Qt::KeyboardModifiers modifiers = {}, Element *elementUnderCursor = nullptr);

    ~AbstractSceneContextMenuEvent();

    Element *elementUnderCursor() const;

private:
    QScopedPointer<AbstractSceneContextMenuEventPrivate> d;
};

class KDSME_VIEW_EXPORT AbstractScene : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QQuickItem *instantiator READ instantiator WRITE setInstantiator NOTIFY instantiatorChanged)
    Q_PROPERTY(ViewState viewState READ viewState NOTIFY viewStateChanged FINAL)
    Q_PROPERTY(Qt::ContextMenuPolicy ContextMenuPolicy READ contextMenuPolicy WRITE setContextMenuPolicy NOTIFY contextMenuPolicyChanged FINAL)

public:
    enum EditTrigger
    {
        NoEditTriggers,
        SelectedClicked
    };
    Q_DECLARE_FLAGS(EditTriggers, EditTrigger)

    enum ViewState
    {
        NoState,
        RefreshState,
    };
    Q_ENUM(ViewState)

    explicit AbstractScene(QQuickItem *parent = nullptr);
    virtual ~AbstractScene();

    virtual void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    virtual void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel() const;

    QQuickItem *instantiator() const;
    void setInstantiator(QQuickItem *instantiator);

    void setEditTriggers(EditTriggers triggers);
    EditTriggers editTriggers() const;

    void setContextMenuPolicy(Qt::ContextMenuPolicy contextMenuPolicy);
    Qt::ContextMenuPolicy contextMenuPolicy() const;

    QObject *itemForIndex(const QModelIndex &index) const;

    ViewState viewState() const;

    QModelIndex currentIndex() const;

public Q_SLOTS:
    void setCurrentIndex(const QModelIndex &index);

protected:
    bool event(QEvent *event) override;

protected Q_SLOTS:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

    /**
     * Called whenever the model's contents layout changed
     * (for example when indices got moved around)
     */
    virtual void layoutChanged();

    void setViewState(KDSME::AbstractScene::ViewState state);

Q_SIGNALS:
    void modelChanged(QAbstractItemModel *model);
    void viewStateChanged(KDSME::AbstractScene::ViewState state);
    void instantiatorChanged(QObject *instantiator);
    void contextMenuPolicyChanged(Qt::ContextMenuPolicy contextMenuPolicy);

    /**
     * This signal is emitted when the scene's contextMenuPolicy is
     * Qt::CustomContextMenu, and the user has requested a context menu on
     * the scene. The position pos is the position of the context menu event
     * that the scene receives. Normally this is in scene coordinates.
     */
    void customContextMenuEvent(KDSME::AbstractSceneContextMenuEvent *event);

private:
    friend AbstractScenePrivate;
    QScopedPointer<AbstractScenePrivate> d;
};

}

Q_DECLARE_METATYPE(KDSME::AbstractScene *)
Q_DECLARE_METATYPE(KDSME::AbstractScene::ViewState)

#endif // ABSTRACTVIEW_H
