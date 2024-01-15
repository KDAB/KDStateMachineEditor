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

Rectangle {
    id: root

    /// Whether it is possible to drag this element around
    property alias enabled: mouseArea.enabled

    property var dragData
    property int dragIndex

    signal dropped(var target)

    width: 6
    height: 6

    color: (mouseArea.pressed ? Theme.currentTheme.highlightFontColor : Theme.currentTheme.highlightBackgroundColor)

    Drag.active: mouseArea.drag.active
    Drag.hotSpot.x: width/2
    Drag.hotSpot.y: height/2

    MouseArea {
        id: mouseArea

        hoverEnabled: true
        cursorShape: Qt.CrossCursor

        anchors.fill: parent
        drag.target: parent

        onReleased: {
            root.dropped(root.Drag.target)
        }

        Component.onCompleted: {
            // drag.smoothed is only there since Qt 5.4.0
            if (typeof drag.smoothed != 'undefined') {
                drag.smoothed = false
            }
        }
    }

}
