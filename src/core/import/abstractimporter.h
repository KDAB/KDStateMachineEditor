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

#ifndef KDSME_ABSTRACTIMPORTER_H
#define KDSME_ABSTRACTIMPORTER_H

#include "kdsme_core_export.h"

#include <QCoreApplication>
#include <QScopedPointer>
#include <QString>

namespace KDSME {

class StateMachine;

class KDSME_CORE_EXPORT AbstractImporter
{
    Q_DECLARE_TR_FUNCTIONS(AbstractImporter)

public:
    AbstractImporter();
    virtual ~AbstractImporter();

    /**
     * Imports the state machine
     *
     * @note Ownership of the object is transferred to the caller
     *
     * @return On success, the pointer to the KDSME::StateMachine instance is returned, otherwise null
     * @sa errorString()
     */
    virtual StateMachine *import() = 0;

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

#endif // KDSME_ABSTRACTIMPORTER_H
