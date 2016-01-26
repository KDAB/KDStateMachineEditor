/*
  DragPointDelegate.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

import QtQuick 2.0

import "qrc:///kdsme/qml/util"

Rectangle {
    id: root

    /// Whether it is possible to drag this element around
    property alias enabled: mouseArea.enabled

    property var dragData
    property int dragIndex

    signal dropped(var target)

    width: 6
    height: 6

    color: (mouseArea.pressed ? Theme.currentTheme.activeHighlightColor : Theme.currentTheme.highlightForegroundColor)

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
