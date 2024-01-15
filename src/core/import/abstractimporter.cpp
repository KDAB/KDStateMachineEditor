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

#include "abstractimporter.h"

using namespace KDSME;

struct AbstractImporter::Private
{
    QString m_errorString;
};


AbstractImporter::AbstractImporter()
    : d(new Private)
{
}

AbstractImporter::~AbstractImporter()
{
}

QString AbstractImporter::errorString() const
{
    return d->m_errorString;
}

void AbstractImporter::setErrorString(const QString &errorString)
{
    d->m_errorString = errorString;
}
