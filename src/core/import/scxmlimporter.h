/*
  scxmlimporter.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
    explicit ScxmlImporter(const QByteArray& data);
    virtual ~ScxmlImporter();

    StateMachine* import() override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // KDSME_SCXMLIMPORTER_H
