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
import QtQuick.Controls 2.12

import com.kdab.kdsme 1.0 as KDSME

import "../util/"

/**
 * Final State representation
 *
 * This is a node in the form of a double-circle
 */
KDSME.StateItem {
    id: root

    readonly property bool active: activeness === 1.0

    readonly property real originalWidth: 20

    opacity: 2*width > originalWidth

    Behavior on opacity {
        NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    RectangularGlowWrapper {
        target: rect
        activeness: activeness
    }

    Rectangle {
        id: rect

        anchors.fill: parent

        color: Theme.currentTheme.stateBackgroundColor
        border.color: Qt.tint(Theme.currentTheme.stateBorderColor, Theme.alphaTint(Theme.currentTheme.stateBorderColor_Active, activeness))
        border.width: (activeness > 0 ? 2 : 1)
        radius: width

        Rectangle {
            anchors {
                fill: parent
                margins: 5
            }

            color: Qt.darker(parent.border.color, 1.5)
            radius: parent.radius*0.5
        }
    }

    RectangularSelectionHandler {
        anchors.fill: parent

        control: parent
    }

    ChannelizedDropArea {
        id: dropArea
        anchors.fill: parent
        keys: ["TransitionType"]
        element: root.element
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
