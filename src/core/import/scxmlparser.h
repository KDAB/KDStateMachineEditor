/*
  scxmlparser.h

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

#ifndef KDSME_IMPORT_SCXMLPARSER_H
#define KDSME_IMPORT_SCXMLPARSER_H

#include "kdsme_core_export.h"

#include <QObject>
#include <QHash>
#include <QXmlStreamReader>

class QIODevice;

namespace KDSME {

class State;
class StateMachine;
class Transition;

/**
 * Parses a SCXML document
 *
 * @see http://www.w3.org/2011/04/SCXML/scxml.xsd
 */
class KDSME_CORE_EXPORT ScxmlParser : public QObject
{
    Q_OBJECT

public:
    explicit ScxmlParser(QObject* parent = 0);

    StateMachine* parse(const QByteArray& data);

    bool hasError() const;
    QString errorString() const;

private:
    /**
     * Start point for XML parsing
     *
     * See http://www.w3.org/2011/04/SCXML/scxml-module-core.xsd for the allowed XML input
     */
    StateMachine* visitScxml();
    void visitTransiton(State* parent);
    void visitState(State* parent);
    void visitInitial(State* parent);
    void visitParallel(State* parent);
    void visitFinal(State* parent);
    void visitHistory(State* parent);

    /// Reset the parser to the initial state (clear cache, etc.)
    void reset();

    /// Create initial state based on current stream reader context
    State* tryCreateInitialState(State* parent);
    /// Create state based on current stream reader context
    template<typename T>
    T* createState(State* parent);
    /// Create Transition
    Transition* createTransition(State* parent, const QString& targetStateId);

    void raiseError(const QString& message);
    void raiseUnexpectedElementError(const QString& context);

    /// Resolves any unresolved targets in transitions
    void resolveTargetStates();

    QXmlStreamReader m_reader;

    /// Map: Transition -> Transition target state id
    QHash<Transition*, QString> m_unresolvedTargetStateIds;
    QHash<QString, State*> m_nameToStateMap;
};

}

#endif // PARSER_H
