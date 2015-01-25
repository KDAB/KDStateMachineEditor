/*
  parsehelper.cpp

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

#include <config-test.h>

#include "parsehelper.h"
#include "scxmlparser.h"
#include "element.h"

#include <QDebug>
#include <QFile>

using namespace KDSME;

StateMachine* ParseHelper::parse(const QByteArray& data)
{
    // parse
    ScxmlParser parser;
    StateMachine* stateMachine = parser.parse(data);
    if (!stateMachine) {
        qDebug() << parser.errorString();
    }
    return stateMachine;
}

StateMachine* ParseHelper::parseFile(const QString& fileName)
{
    const QByteArray data = readFile(fileName);
    return parse(data);
}

QByteArray ParseHelper::readFile(const QString& fileName)
{
    QFile file(QString("%1/%2").arg(TEST_DATA_DIR, fileName));
    if (!file.exists()) {
        qWarning() << "File does not exist:" << file.fileName();
        return QByteArray();
    }
    bool success = file.open(QIODevice::ReadOnly);
    if (!success)
        return QByteArray();

    const QByteArray data = file.readAll();
    return data;
}
