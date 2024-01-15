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

import com.kdab.kdsme 1.0 as KDSME

Item {
    id: root

    property var control

    signal clicked
    signal doubleClicked

    Rectangle {
        id: rect

        anchors.fill: parent

        border {
            color: Theme.currentTheme.highlightBackgroundColor
            width: 2
        }
        color: "transparent"
        visible: control ?
            control.element.flags & KDSME.Element.ElementIsSelectable && control.element.selected :
            false
    }

    Item {
        id: invisibleDragTarget

        Drag.active: mouseArea.drag.active
        Drag.hotSpot.x: width/2
        Drag.hotSpot.y: height/2

        width: parent.width
        height: parent.height
    }

    MouseArea {
        id: mouseArea

        anchors.fill: invisibleDragTarget

        drag.target: control.element.flags & KDSME.Element.ElementIsDragEnabled ?
            invisibleDragTarget :
            null

        onClicked: {
            scene.currentItem = control.element;
            root.clicked();
        }
        onDoubleClicked: {
            root.doubleClicked();
        }

        function move(dx, dy) {
            var cmd = KDSME.CommandFactory.modifyElement(control.element);
            cmd.moveBy(dx, dy);
            commandController.push(cmd);
        }

        onXChanged: {
            move(x, 0);
        }
        onYChanged: {
            move(0, y);
        }
    }

}
