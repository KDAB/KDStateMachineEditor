/*
  UmlElement.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
import QtQuick.Controls 1.0

import com.kdab.kdsme 1.0

import "qrc:///kdsme/qml/util/"

Item {
    id: root

    property variant item: null

    property string name: (item.element.label != "" ? item.element.label : qsTr("Unnamed"))
    property string toolTipText: ""
    property bool selectable: true
    property bool selected: item.selected
    property bool resizable: false
    property bool movable: true
    property alias hovered: mouseArea.containsMouse
    property alias mouseAreaMask: mouseArea.mask
    property real activeness: 0

    /**
     * Delegate which is marked as visible as soon as the item is selected
     *
     * This is a rectangular shaped item by default
     */
    property Component selectionComponent: Rectangle {
        border {
            color: Theme.highlightForegroundColor
            width: 2
        }
        color: "transparent"

        MoveArea {
            enabled: root.movable

            width: root.width
            height: root.height

            function move(dx, dy) {
                var cmd = CommandFactory.modifyLayoutItem(root.item);
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

    /**
     * Delegate which is marked as visible as soon as the item is selected
     *
     * This is a rectangular shaped item by default
     */
    property Component draggingComponent: DragPointGroup {
        function adjusted(rect, xp1, yp1, xp2, yp2)
        {
            rect.x += xp1;
            rect.y += yp1;
            rect.width  += xp2 - xp1;
            rect.height += yp2 - yp1;
            return rect;
        }

        points: [
            DragPoint { x: x;       y: y },
            DragPoint { x: width;   y: y },
            DragPoint { x: width;   y: height },
            DragPoint { x: x;       y: height }
        ]

        onChanged: {
            var item = root.item;
            var point = pointAt(index);
            var rect = Qt.rect(item.pos.x, item.pos.y, item.width, item.height);
            var cmd = CommandFactory.modifyLayoutItem(root.item)
            switch (index) {
            // TODO: Implement me
            }
            commandController.push(cmd);
        }
    }

    signal clicked()

    anchors {
        left: parent.left
        top: parent.top
    }

    width: item.width
    height: item.height
    visible: item.visible

    onClicked: view.setCurrentItem(object)

    Loader {
        id: selectionComponentLoader
        anchors.fill: parent
        z: 1
        visible: root.selectable && root.selected
        asynchronous: true
        sourceComponent: root.selectionComponent
    }

    Loader {
        id: draggingComponentLoader
        anchors.fill: parent
        z: 1
        visible: root.resizable && root.selected
        asynchronous: true
        sourceComponent: root.draggingComponent
    }

    /*
    ToolTip {
        id: toolTip

        targetX: mouseArea.mouseX
        targetY: mouseArea.mouseY

        hovered: root.hovered
        text: root.toolTipText
    }
    */

    MaskedMouseArea {
        id: mouseArea

        anchors.fill: parent

        enabled: root.selectable && !root.selected
        tolerance: 5

        onClicked: {
            root.clicked()
        }
    }
}
