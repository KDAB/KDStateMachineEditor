/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.0

Loader {
    id: root

    property Item target
    property real activeness

    readonly property bool isQt6: {
        try {
            return Qt.createQmlObject("import Qt5Compat.GraphicalEffects; RectangularGlow {}", root, "dynamicItem") !== null;
        } catch (error) {
            return false;
        }
    }

    anchors.fill: target

    source: isQt6 ? "qrc:/kdsme/qml/util/RectangularGlowQt6.qml" : "qrc:/kdsme/qml/util/RectangularGlowQt5.qml"
}
