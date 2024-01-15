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

import "../util/"

Item {
    id: root

    property var control

    readonly property bool active: control.activeness === 1.0

    anchors.fill: parent

    RectangularGlowWrapper {
        target: rect
        activeness: control.activeness
    }

    Behavior on opacity {
        NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    Rectangle {
        id: rect

        anchors.fill: parent

        color: Qt.tint(Theme.currentTheme.stateBackgroundColor, Theme.alphaTint(Theme.currentTheme.stateBackgroundColor_Active, activeness))
        border.color: Qt.tint(Theme.currentTheme.stateBorderColor, Theme.alphaTint(Theme.currentTheme.stateBorderColor_Active, activeness))
        border.width: (activeness > 0 ? 2 : 1)
        radius: 5

        Text {
            id: label

            anchors.centerIn: parent
            width: parent.width

            text: control.element.label
            renderType: Text.NativeRendering
            color: (activeness > 0.5 ? Theme.currentTheme.highlightFontColor : Theme.currentTheme.stateLabelFontColor)
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }
    }

    RectangularSelectionHandler {
        anchors.fill: parent

        control: parent.control
    }

    ChannelizedDropArea {
        id: dropArea
        anchors.fill: parent
        keys: ["StateType", "TransitionType", "external"]
        element: control.element
    }

    states: [
        State {
            when: dropArea.containsDrag
            PropertyChanges {
                target: rect
                color: "#DDDDDD"
            }
        }
    ]

}
