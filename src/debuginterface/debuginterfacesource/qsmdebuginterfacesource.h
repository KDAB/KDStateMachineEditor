/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
    void setQStateMachine(QStateMachine* machine);

    /**
     * Publish this object on the QtRemoteObjects bus
     *
     * @sa QRemoteObjectNode::enableRemoting()
     */
    QObject* remoteObjectSource() const;

private:
    class Private;
    QScopedPointer<Private> d;
};

}

#endif // KDSME_QSMDEBUGINTERFACESOURCE_H
