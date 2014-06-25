/*
  layoutitemmodel.cpp

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

#include "layoutitemmodel.h"

#include "element.h"
#include "layoutitem.h"
#include "objecthelper.h"
#include "view.h"

#include <QColor>
#include <QDebug>
#include <QPalette>

using namespace ObjectHelper;
using namespace KDSME;

namespace {

QString attributesString(const LayoutItem* item)
{
    if (auto i = qobject_cast<const StateLayoutItem*>(item)) {
        return QString("pos=%1, width=%2, height=%3").arg(toString(i->pos())).arg(i->width()).arg(i->height());
    } else if (auto i = qobject_cast<const TransitionLayoutItem*>(item)) {
        const QPointF startPos = i->shape().pointAtPercent(0);
        const QPointF endPos = i->shape().pointAtPercent(1);
        const qreal length = i->shape().length();
        return QString("pos=%1, start=%2, end=%3, path length=%4, label rect:%5")
            .arg(toString(i->pos())).arg(toString(startPos)).arg(toString(endPos)).arg(length).arg(toString(i->labelBoundingRect()));
    }
    return QString();
}

}

LayoutItemModel::LayoutItemModel(QObject* parent)
    : ObjectTreeModel(parent)
{
}

View* LayoutItemModel::view() const
{
    return m_view;
}

void LayoutItemModel::setView(View* view)
{
    if (m_view == view)
        return;

    if (m_view) {
        disconnect(m_view.data(), &View::contentsAboutToBeChanged, this, &LayoutItemModel::slotBeginResetModel);
        disconnect(m_view.data(), &View::contentsChanged, this, &LayoutItemModel::slotEndResetModel);
        disconnect(m_view.data(), &View::contentsUpdated, this, &LayoutItemModel::slotModelContentUpdated);
        clear();
    }
    m_view = view;
    if (m_view) {
        connect(m_view.data(), &View::contentsAboutToBeChanged, this, &LayoutItemModel::slotBeginResetModel);
        connect(m_view.data(), &View::contentsChanged, this, &LayoutItemModel::slotEndResetModel);
        connect(m_view.data(), &View::contentsUpdated, this, &LayoutItemModel::slotModelContentUpdated);
        setRootObjects(QList<QObject*>() << m_view->rootLayoutItem());
    }
    emit viewChanged(view);
}

void LayoutItemModel::slotBeginResetModel()
{
    clear();
}

void LayoutItemModel::slotEndResetModel()
{
    setRootObjects(QList<QObject*>() << m_view->rootLayoutItem());
}

void LayoutItemModel::slotModelContentUpdated(LayoutItem* item)
{
    // TODO: Emit dataChanged properly

    
}

QVariant LayoutItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    LayoutItem* item = qobject_cast<LayoutItem*>(ObjectTreeModel::data(index, ObjectRole).value<QObject*>());
    Q_ASSERT(item);
    if (!item)
        return ObjectTreeModel::data(index, role);


    if (role == Qt::DisplayRole) {
        const int column = index.column();
        switch (column) {
        case NameColumn:
            return item->toDisplayString();
        case ElementColumn:
            return (item->element() ? item->element()->toDisplayString() : QString("0x0"));
        case AttributesColumn:
            return attributesString(item);
        default:
            return ObjectTreeModel::data(index, role);
        }
    }
    if (role == Qt::TextColorRole && !item->isVisible()) {
        QPalette p;
        return p.color(QPalette::Disabled, QPalette::Text);
    }
    return ObjectTreeModel::data(index, role);
}

QVariant LayoutItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0 || section >= columnCount())
        return QVariant();

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case NameColumn:
            return tr("Item");
        case ElementColumn:
            return tr("Element");
        case AttributesColumn:
            return tr("Attributes");
        default:
            return QVariant();
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

int LayoutItemModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return _LastColumn;
}

#include "moc_layoutitemmodel.cpp"
