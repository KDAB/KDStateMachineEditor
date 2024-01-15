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

#ifndef KDSME_SCXMLIMPORTER_H
#define KDSME_SCXMLIMPORTER_H

#include "kdsme_core_export.h"

#include "abstractimporter.h"

namespace KDSME {

class StateMachine;

/**
 * Parses a SCXML document
 *
 * @see http://www.w3.org/2011/04/SCXML/scxml.xsd
 */
class KDSME_CORE_EXPORT ScxmlImporter : public AbstractImporter
{
public:
    explicit ScxmlImporter(const QByteArray &data);
    virtual ~ScxmlImporter();

    StateMachine *import() override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

/**
 * @example scxmlimport/main.cpp
 *
 * This examples shows how to import a @ref KDSME::StateMachine by importing a SCXML document
 */

#endif // KDSME_SCXMLIMPORTER_H
