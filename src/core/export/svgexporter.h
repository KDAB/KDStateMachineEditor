/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2015-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
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

    bool exportMachine(StateMachine* machine) override;
private:
    friend class SvgExporterPrivate;
    QScopedPointer<SvgExporterPrivate> d;
};

}

#endif // KDSME_SVGEXPORTER_H
