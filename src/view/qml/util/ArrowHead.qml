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

import QtQuick 2.0

import com.kdab.kdsme 1.0

/**
 * QML Arrow head
 *
 * Visual representation:
 *
 * (0,0)
 * |\
 * |  \
 * |    \ (width, height/2)
 * |    /
 * |  /
 * |/
 * (0, height)
 */
Primitive {
    id: root

    property bool filled: true

    geometry: Geometry {
        data: [
            0, 0,
            width, height/2,
            0, height
        ]
        type: Geometry.Point2DType
        drawingMode: Geometry.TriangleStrip
    }
}
