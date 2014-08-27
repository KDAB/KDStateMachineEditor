/*
  propertyeditor.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef KDSME_WIDGETS_PROPERTYEDITOR_H
#define KDSME_WIDGETS_PROPERTYEDITOR_H

#include "kdsme_view_export.h"

#include <QStackedWidget>

class QItemSelectionModel;
class QModelIndex;

namespace KDSME {

class CommandController;
class Element;

class KDSME_VIEW_EXPORT PropertyEditor : public QStackedWidget
{
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget *parent = 0);
    ~PropertyEditor();

    void setSelectionModel(QItemSelectionModel* selectionModel);
    void setCommandController(CommandController *cmdController);

private:
    struct Private;
    QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void updateSimpleProperty());
    Q_PRIVATE_SLOT(d, void setInitalState(const QString& label));
    Q_PRIVATE_SLOT(d, void setDefaultState(const QString& label));
    Q_PRIVATE_SLOT(d, void setSourceState(const QString& label));
    Q_PRIVATE_SLOT(d, void setTargetState(const QString& label));
    Q_PRIVATE_SLOT(d, void childModeChanged());
    Q_PRIVATE_SLOT(d, void currentChanged(const QModelIndex &current, const QModelIndex &previous));
    Q_PRIVATE_SLOT(d, void loadFromCurrentElement());
};

} // namespace KDSME

#endif // PROPERTYEDITOR_H
