/*
  layoutitemmodel.h

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

#ifndef KDSME_LAYOUT_LAYOUTITEMMODEL_H
#define KDSME_LAYOUT_LAYOUTITEMMODEL_H

#include "objecttreemodel.h"
#include "kdsme_core_export.h"

#include <QAbstractItemModel>
#include <QPointer>

namespace KDSME {

class LayoutItem;
class View;

class KDSME_CORE_EXPORT LayoutItemModel : public ObjectTreeModel
{
    Q_OBJECT
    Q_PROPERTY(KDSME::View* view READ view WRITE setView NOTIFY viewChanged)

public:
    enum Column {
        NameColumn,
        ElementColumn,
        AttributesColumn,
        _LastColumn
    };

    explicit LayoutItemModel(QObject* parent = 0);

    View* view() const;
    void setView(View* view);

    virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void slotBeginResetModel();
    void slotEndResetModel();

    void slotModelContentUpdated(LayoutItem* item);

Q_SIGNALS:
    void viewChanged(View* view);

private:
    QPointer<View> m_view;
};

}

#endif // LAYOUTINFORMATIONMODEL_H
