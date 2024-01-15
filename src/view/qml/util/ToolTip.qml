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

    property string text: ""
    property real targetX: 0
    property real targetY: 0
    property bool hovered: false
    property int delay: 500 // ms

    function calculatePosition() {
        x = root.targetX
        y = root.targetY
    }

    onHoveredChanged: {
        if (!hovered) {
            visible = 0
        }
    }

    onVisibleChanged: {
        if (visible) {
            calculatePosition()
        }
    }

    // internal
    readonly property int margin: 5

    width: dummyItem.width + margin*2
    height: dummyItem.height + margin*2
    z: parent.z+1
    visible: false

    color: "lightsteelblue"

    Timer {
        interval: root.delay
        running: root.hovered && root.text !== ""

        onTriggered: {
            root.visible = true
        }
    }

    // Work-around: Used to get the size of the label
    // If we depended on the size of 'label', we'd get binding loops, because
    // anchors.centerIn is used for this particular element
    Text {
        id: dummyItem
        text: root.text
        renderType: Text.NativeRendering
        visible: false
    }

    Text {
        id: label

        anchors.centerIn: parent
        text: root.text
        renderType: Text.NativeRendering
        // QTBUG? anchors.centerIn + multiline text + word-wrap breaks layouting
        //wrapMode: Text.WordWrap
    }
}
