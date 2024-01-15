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

#ifndef KDSME_EXPORT_ABSTRACTEXPORTER_H
#define KDSME_EXPORT_ABSTRACTEXPORTER_H

#include "kdsme_core_export.h"

#include <QScopedPointer>
#include <QString>

namespace KDSME {

class StateMachine;

class KDSME_CORE_EXPORT AbstractExporter
{
public:
    AbstractExporter();
    virtual ~AbstractExporter();

    /**
     * Exports the state machine @p machine into the desired format
     *
     * @return True on success, otherwise false
     * @sa errorString()
     */
    virtual bool exportMachine(StateMachine *machine) = 0;

    /**
     * Returns a human readable description of the last error that occurred.
     */
    QString errorString() const;

protected:
    /**
     * Sets the human readable description of the last device error that occurred to @p errorString
     */
    void setErrorString(const QString &errorString);

private:
    struct Private;
    QScopedPointer<Private> d;
};

}


/**
 * @example export/main.cpp
 *
 * This is an example of how to use the inheriters of the @ref KDSME::AbstractExporter class.
 */

#endif
