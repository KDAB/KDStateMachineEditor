/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_QSMDEBUGINTERFACESOURCE_H
#define KDSME_QSMDEBUGINTERFACESOURCE_H

#include "kdsme_debuginterfacesource_export.h"

#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QStateMachine;
QT_END_NAMESPACE

namespace KDSME {

class KDSME_DEBUGINTERFACESOURCE_EXPORT QsmDebugInterfaceSource
{
public:
    QsmDebugInterfaceSource();
    virtual ~QsmDebugInterfaceSource();

    QStateMachine *qStateMachine() const;
    void setQStateMachine(QStateMachine *machine);

    /**
     * Publish this object on the QtRemoteObjects bus
     *
     * @sa QRemoteObjectNode::enableRemoting()
     */
    QObject *remoteObjectSource() const;

private:
    class Private;
    QScopedPointer<Private> d;
};

}

#endif // KDSME_QSMDEBUGINTERFACESOURCE_H
