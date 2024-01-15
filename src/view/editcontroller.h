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
    explicit EditController(StateMachineView *parent = nullptr);
    ~EditController();

    bool editModeEnabled() const;
    void setEditModeEnabled(bool editModeEnabled);

public Q_SLOTS:
    bool sendDragEnterEvent(KDSME::Element *sender, KDSME::Element *target, const QPoint &pos, const QList<QUrl> &urls);
    bool sendDropEvent(KDSME::Element *sender, KDSME::Element *target, const QPoint &pos, const QList<QUrl> &urls);

Q_SIGNALS:
    void editModeEnabledChanged(bool editModeEnabled);

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // EDITCONTROLLER_H
