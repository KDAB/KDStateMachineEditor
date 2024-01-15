/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_QSCXMLDEBUGINTERFACESOURCE_H
#define KDSME_QSCXMLDEBUGINTERFACESOURCE_H

#include "kdsme_debuginterfacesource_export.h"

#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QScxmlStateMachine;
QT_END_NAMESPACE

namespace KDSME {

class KDSME_DEBUGINTERFACESOURCE_EXPORT QScxmlDebugInterfaceSource
{
public:
    QScxmlDebugInterfaceSource();
    virtual ~QScxmlDebugInterfaceSource();

    QScxmlStateMachine *qScxmlStateMachine() const;
    void setQScxmlStateMachine(QScxmlStateMachine *machine);

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

#endif // KDSME_QSCXMLDEBUGINTERFACESOURCE_H
