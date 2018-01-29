/*
  scxmlexporter.h

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

#ifndef KDSME_EXPORT_SCXMLEXPORTER_H
#define KDSME_EXPORT_SCXMLEXPORTER_H

#include "abstractexporter.h"

QT_BEGIN_NAMESPACE
class QByteArray;
class QIODevice;
QT_END_NAMESPACE

namespace KDSME {

class StateMachine;

class KDSME_CORE_EXPORT ScxmlExporter : public AbstractExporter
{
public:
    explicit ScxmlExporter(QByteArray* array);
    explicit ScxmlExporter(QIODevice* device);
    virtual ~ScxmlExporter();

    /**
     * Exports state machine @p machine in SCXML format
     *
     * @note For exporting to work, *all* states must-have unique labels set
     */
    virtual bool exportMachine(StateMachine* machine) override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // SCXMLEXPORT_H
