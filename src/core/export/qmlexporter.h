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

#ifndef KDSME_EXPORT_QMLEXPORTER_H
#define KDSME_EXPORT_QMLEXPORTER_H

#include "abstractexporter.h"

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

namespace KDSME {

class StateMachine;

class KDSME_CORE_EXPORT QmlExporter : public AbstractExporter
{
public:
    explicit QmlExporter(QByteArray *array);
    explicit QmlExporter(QIODevice *device);
    virtual ~QmlExporter();

    int indent() const;
    void setIndent(int indent);

    virtual bool exportMachine(StateMachine *machine) override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // QMLEXPORTER_H
