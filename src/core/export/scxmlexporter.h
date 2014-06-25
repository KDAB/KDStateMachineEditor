/*
  scxmlexporter.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include <QXmlStreamWriter>

class QByteArray;

namespace KDSME {

class State;
class StateMachine;
class Transition;

class KDSME_CORE_EXPORT ScxmlExporter : public AbstractExporter
{
public:
    explicit ScxmlExporter(QByteArray* array);
    explicit ScxmlExporter(QIODevice* device);

    /**
     * Exports state machine @p machine in SCXML format
     *
     * @note For exporting to work, *all* states must-have unique labels set
     */
    virtual bool exportMachine(StateMachine* machine) Q_DECL_OVERRIDE;

private:
    void init();

    bool writeStateMachine(StateMachine* machine);
    bool writeState(State* state);
    bool writeStateInner(State* state);
    bool writeTransition(Transition* transition);

    QXmlStreamWriter m_writer;
};

}

#endif // SCXMLEXPORT_H
