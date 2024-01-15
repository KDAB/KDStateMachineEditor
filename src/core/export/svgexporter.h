/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_SVGEXPORTER_H
#define KDSME_SVGEXPORTER_H

#include "abstractexporter.h"

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

namespace KDSME {

class SvgExporterPrivate;

class KDSME_CORE_EXPORT SvgExporter : public AbstractExporter
{
public:
    explicit SvgExporter(QIODevice *ioDevice);
    ~SvgExporter();

    bool exportMachine(StateMachine *machine) override;

private:
    friend class SvgExporterPrivate;
    QScopedPointer<SvgExporterPrivate> d;
};

}

#endif // KDSME_SVGEXPORTER_H
