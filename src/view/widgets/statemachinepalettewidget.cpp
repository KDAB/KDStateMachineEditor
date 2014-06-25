/*
  statemachinepalettewidget.cpp

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

#include "statemachinepalettewidget.h"

#include "element.h"
#include "kdsmeconstants.h"

#include <QDebug>
#include <QListView>
#include <QMimeData>
#include <QUrl>
#include <QVBoxLayout>

using namespace KDSME;

namespace KDSME {

class PaletteModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        ElementTypeRole = Qt::UserRole + 1
    };

    explicit PaletteModel(QObject* parent = 0);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QMimeData* mimeData(const QModelIndexList& indexes) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

private:
    struct Entry
    {
        Entry(Element::Type type_, const QString& iconName_, const QString& name_)
            : type(type_)
            , iconName(iconName_)
            , name(name_)
        {}
        Entry()
            : type(Element::ElementType)
        {}

        Element::Type type;
        QString iconName;
        QString name;
    };

    QVector<Entry> m_entries;
};

}

PaletteModel::PaletteModel(QObject* parent)
    : QAbstractListModel(parent)
{
    m_entries << Entry(Element::StateType, ":/kdsme/icons/state.png", tr("State"));
    // we only can edit one state machine at a time, so don't add this one here
    m_entries << Entry(Element::FinalStateType, ":/kdsme/icons/final_state.png", tr("Final State"));

    m_entries << Entry(Element::HistoryStateType, ":/kdsme/icons/shallow_history.png", tr("History State"));

    m_entries << Entry(Element::TransitionType, ":/kdsme/icons/transition.png", tr("Transition"));

}

int PaletteModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_entries.count();
}

QVariant PaletteModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size())
        return QVariant();

    const Entry& entry = m_entries.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
        return entry.name;
    case Qt::DecorationRole: {
        return QIcon(entry.iconName);
    }
    case ElementTypeRole:
        return QVariant::fromValue<Element::Type>(entry.type);
    default:
        return QVariant();
    }
}

QMimeData* PaletteModel::mimeData(const QModelIndexList& indexes) const
{
    Q_ASSERT(indexes.size() == 1); // we only allow single selection

    const QModelIndex index = indexes.first();
    auto type = index.data(ElementTypeRole).value<Element::Type>();

    QMimeData* mimeData = new QMimeData;
    mimeData->setData("external", "");
    mimeData->setUrls(QList<QUrl>() << QString("%1:Element/%2").arg(KDSME_QML_URI_PREFIX, Element::typeToString(type)));
    return mimeData;
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex& index) const
{
    auto flags = QAbstractListModel::flags(index);
    return Qt::ItemIsDragEnabled | flags;
}

StateMachinePaletteWidget::StateMachinePaletteWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QListView* view = new QListView(this);
    view->setDragEnabled(true);
    view->setViewMode(QListView::IconMode);
    view->setFlow(QListView::LeftToRight);
    view->setGridSize(QSize(128, 48));
    view->setSpacing(10);
    view->setIconSize(QSize(32, 32));
    view->viewport()->setAcceptDrops(false); // disallow internal drops
    layout->addWidget(view);
    view->setModel(new PaletteModel);
}

#include "statemachinepalettewidget.moc"
