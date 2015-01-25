/*
  editcontroller.h

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

#ifndef KDSME_EDITCONTROLLER_H
#define KDSME_EDITCONTROLLER_H

#include "kdsme_view_export.h"

#include "abstractcontroller.h"

namespace KDSME {

class Element;

class KDSME_VIEW_EXPORT EditController : public AbstractController
{
    Q_OBJECT
    Q_PROPERTY(bool editModeEnabled READ editModeEnabled WRITE setEditModeEnabled NOTIFY editModeEnabledChanged)

public:
    explicit EditController(StateMachineView* parent = nullptr);
    virtual ~EditController();

    bool editModeEnabled() const;
    void setEditModeEnabled(bool editModeEnabled);

public Q_SLOTS:
    bool sendDragEnterEvent(KDSME::Element* sender, KDSME::Element* target, const QPoint& pos, const QList<QUrl>& urls);
    bool sendDropEvent(KDSME::Element* sender, KDSME::Element* target, const QPoint& pos, const QList<QUrl>& urls);

Q_SIGNALS:
    void editModeEnabledChanged(bool editModeEnabled);

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // EDITCONTROLLER_H
