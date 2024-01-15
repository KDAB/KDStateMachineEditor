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

#include "abstractexporter.h"

using namespace KDSME;

struct AbstractExporter::Private
{
    QString m_errorString;
};

AbstractExporter::AbstractExporter()
    : d(new Private)
{
}

AbstractExporter::~AbstractExporter()
{
}

QString AbstractExporter::errorString() const
{
    return d->m_errorString;
}

void AbstractExporter::setErrorString(const QString &errorString)
{
    d->m_errorString = errorString;
}
